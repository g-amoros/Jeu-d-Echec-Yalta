#include "ia/IAMinMax.h"

#include "modele/TypePiece.h"

#include <algorithm>
#include <future>
#include <limits>
#include <random>
#include <thread>
#include <utility>
#include <vector>

namespace yalta {

namespace {

int valeurPiece(TypePiece type) {
    switch (type) {
        case TypePiece::PION:     return 10;
        case TypePiece::CAVALIER: return 30;
        case TypePiece::FOU:      return 30;
        case TypePiece::TOUR:     return 50;
        case TypePiece::REINE:    return 90;
        case TypePiece::ROI:      return 10000;
    }
    return 0;
}

// Captures en premier, puis par valeur décroissante de la victime (MVV simplifié).
void ordonnerCoups(std::vector<Coup>& coups, const Partie& partie) {
    std::stable_sort(coups.begin(), coups.end(), [&](const Coup& a, const Coup& b) {
        const Piece* cibleA = partie.plateau().pieceEn(a.destination);
        const Piece* cibleB = partie.plateau().pieceEn(b.destination);
        const int scoreA = cibleA ? valeurPiece(cibleA->getType()) : -1;
        const int scoreB = cibleB ? valeurPiece(cibleB->getType()) : -1;
        return scoreA > scoreB;
    });
}

} // namespace

IAMinMax::IAMinMax(int profondeur) : profondeur_(std::max(1, profondeur)) {}

int IAMinMax::evaluer(const Partie& partie, Couleur joueurIA) {
    int score = 0;
    for (const auto& [position, piece] : partie.plateau().cases()) {
        (void)position;
        if (!piece) continue;
        const int valeur = valeurPiece(piece->getType());
        score += (piece->getCouleur() == joueurIA) ? valeur : -valeur;
    }
    return score;
}

// Alpha-bêta récursif (modèle paranoïaque : les adversaires forment une coalition).
int IAMinMax::minmax(Partie partie, int profondeur, int alpha, int beta,
                     bool maximisant, Couleur joueurIA) const {
    if (profondeur == 0 || partie.estTerminee()) {
        return evaluer(partie, joueurIA);
    }

    auto coups = partie.coupsLegaux();
    if (coups.empty()) {
        return evaluer(partie, joueurIA);
    }

    ordonnerCoups(coups, partie);

    if (maximisant) {
        int valeur = std::numeric_limits<int>::min();
        for (const auto& coup : coups) {
            Partie copie = partie;
            if (!copie.jouerCoup(coup)) continue;
            const bool prochainMax = (copie.joueurActif().couleur == joueurIA);
            valeur = std::max(valeur,
                minmax(std::move(copie), profondeur - 1, alpha, beta, prochainMax, joueurIA));
            alpha = std::max(alpha, valeur);
            if (alpha >= beta) break;   // coupure bêta
        }
        return valeur;
    }

    int valeur = std::numeric_limits<int>::max();
    for (const auto& coup : coups) {
        Partie copie = partie;
        if (!copie.jouerCoup(coup)) continue;
        const bool prochainMax = (copie.joueurActif().couleur == joueurIA);
        valeur = std::min(valeur,
            minmax(std::move(copie), profondeur - 1, alpha, beta, prochainMax, joueurIA));
        beta = std::min(beta, valeur);
        if (alpha >= beta) break;       // coupure alpha
    }
    return valeur;
}

Coup IAMinMax::meilleurCoup(const Partie& partie) const {
    const Couleur joueurIA = partie.joueurActif().couleur;
    auto coups = partie.coupsLegaux();
    if (coups.empty()) return {};

    // Tri initial au niveau racine pour améliorer les coupures inter-threads.
    ordonnerCoups(coups, partie);

    // Parallélisme limité à hardware_concurrency pour éviter la contention.
    const unsigned int nbThreads =
        std::max(1u, std::thread::hardware_concurrency());
    const std::size_t n = coups.size();

    std::vector<int> scores(n, std::numeric_limits<int>::min());

    // Découpage en tranches : chaque thread traite un sous-ensemble de coups.
    std::vector<std::future<void>> futures;
    futures.reserve(nbThreads);

    const std::size_t tranche = (n + nbThreads - 1) / nbThreads;

    for (unsigned int t = 0; t < nbThreads; ++t) {
        const std::size_t debut = t * tranche;
        if (debut >= n) break;
        const std::size_t fin = std::min(debut + tranche, n);

        futures.push_back(std::async(std::launch::async,
            [this, &partie, &coups, &scores, joueurIA, debut, fin]() {
                for (std::size_t i = debut; i < fin; ++i) {
                    Partie copie = partie;
                    if (!copie.jouerCoup(coups[i])) continue;
                    const bool prochainMax = (copie.joueurActif().couleur == joueurIA);
                    scores[i] = minmax(std::move(copie), profondeur_ - 1,
                                       std::numeric_limits<int>::min(),
                                       std::numeric_limits<int>::max(),
                                       prochainMax, joueurIA);
                }
            }));
    }

    for (auto& f : futures) f.get();

    // Tiebreak aléatoire parmi les ex-æquo pour éviter les cycles.
    int meilleureValeur = std::numeric_limits<int>::min();
    std::vector<Coup> exAequo;
    for (std::size_t i = 0; i < n; ++i) {
        if (scores[i] > meilleureValeur) {
            meilleureValeur = scores[i];
            exAequo.clear();
            exAequo.push_back(coups[i]);
        } else if (scores[i] == meilleureValeur) {
            exAequo.push_back(coups[i]);
        }
    }

    static thread_local std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<std::size_t> dist(0, exAequo.size() - 1);
    return exAequo[dist(rng)];
}

} // namespace yalta
