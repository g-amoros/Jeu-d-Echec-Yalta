#include "ia/IAMinMax.h"

#include "modele/TypePiece.h"

#include <algorithm>
#include <future>
#include <limits>
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

} // namespace

IAMinMax::IAMinMax(int profondeur) : profondeur_(std::max(1, profondeur)) {}

int IAMinMax::evaluer(const Partie& partie, Couleur joueurIA) {
    int score = 0;

    for (const auto& [position, piece] : partie.plateau().cases()) {
        (void)position;
        if (!piece) {
            continue;
        }

        const int valeur = valeurPiece(piece->getType());
        score += (piece->getCouleur() == joueurIA) ? valeur : -valeur;
    }

    return score;
}

int IAMinMax::minmax(Partie partie, int profondeur, bool maximisant, Couleur joueurIA) const {
    if (profondeur == 0 || partie.estTerminee()) {
        return evaluer(partie, joueurIA);
    }

    const auto coups = partie.coupsLegaux();
    if (coups.empty()) {
        return evaluer(partie, joueurIA);
    }

    if (maximisant) {
        int meilleureValeur = std::numeric_limits<int>::min();

        for (const auto& coup : coups) {
            Partie copie = partie;
            if (!copie.jouerCoup(coup)) {
                continue;
            }

            const bool prochainMaximise = (copie.joueurActif().couleur == joueurIA);
            const int valeur = minmax(std::move(copie), profondeur - 1, prochainMaximise, joueurIA);
            meilleureValeur = std::max(meilleureValeur, valeur);
        }

        return meilleureValeur;
    }

    int meilleureValeur = std::numeric_limits<int>::max();

    for (const auto& coup : coups) {
        Partie copie = partie;
        if (!copie.jouerCoup(coup)) {
            continue;
        }

        const bool prochainMaximise = (copie.joueurActif().couleur == joueurIA);
        const int valeur = minmax(std::move(copie), profondeur - 1, prochainMaximise, joueurIA);
        meilleureValeur = std::min(meilleureValeur, valeur);
    }

    return meilleureValeur;
}

Coup IAMinMax::meilleurCoup(const Partie& partie) const {
    const Couleur joueurIA = partie.joueurActif().couleur;
    const auto coups = partie.coupsLegaux();
    if (coups.empty()) {
        return {};
    }

    std::vector<std::future<int>> evaluations;
    evaluations.reserve(coups.size());

    for (const auto& coup : coups) {
        evaluations.push_back(std::async(std::launch::async, [this, &partie, joueurIA, coup]() {
            Partie copie = partie;
            if (!copie.jouerCoup(coup)) {
                return std::numeric_limits<int>::min();
            }

            const bool prochainMaximise = (copie.joueurActif().couleur == joueurIA);
            return minmax(std::move(copie), profondeur_ - 1, prochainMaximise, joueurIA);
        }));
    }

    Coup meilleur = coups.front();
    int meilleureValeur = std::numeric_limits<int>::min();

    for (std::size_t index = 0; index < coups.size(); ++index) {
        const int valeur = evaluations[index].get();
        if (valeur > meilleureValeur) {
            meilleureValeur = valeur;
            meilleur = coups[index];
        }
    }

    return meilleur;
}

} // namespace yalta
