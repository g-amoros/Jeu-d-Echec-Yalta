#include "Partie.h"

#include "PieceFactory.h"

#include <algorithm>

namespace yalta {

Partie::Partie() {
    pieces_ = PieceFactory::dispositionInitiale();
    for (const auto& p : pieces_) {
        plateau_.poser(p->getPosition(), p.get());
    }
    joueurs_ = {
        Joueur{Couleur::BLANC},
        Joueur{Couleur::NOIR},
        Joueur{Couleur::ROUGE}
    };
}

Piece* Partie::trouverRoi(Couleur c) const {
    for (const auto& p : pieces_) {
        if (p->getType() == TypePiece::ROI && p->getCouleur() == c) {
            return p.get();
        }
    }
    return nullptr;
}

bool Partie::estEnEchec(Couleur c) const {
    Piece* roi = trouverRoi(c);
    if (!roi) return false;
    return plateau_.estMenacee(roi->getPosition(), c);
}

bool Partie::aUnCoupLegal(Couleur c) const {
    for (const auto& p : pieces_) {
        if (p->getCouleur() != c) continue;
        for (Position dest : p->coupsPossibles(plateau_)) {
            // Simulation : on joue puis on annule pour vérifier l'échec.
            Position depart = p->getPosition();
            Piece*   prise  = plateau_.pieceEn(dest);
            const_cast<Plateau&>(plateau_).deplacer(depart, dest);
            p->setPosition(dest);
            bool enEchec = estEnEchec(c);
            // rollback
            const_cast<Plateau&>(plateau_).deplacer(dest, depart);
            p->setPosition(depart);
            if (prise) const_cast<Plateau&>(plateau_).poser(dest, prise);
            if (!enEchec) return true;
        }
    }
    return false;
}

bool Partie::estMat(Couleur c) const {
    return estEnEchec(c) && !aUnCoupLegal(c);
}

bool Partie::estPat(Couleur c) const {
    return !estEnEchec(c) && !aUnCoupLegal(c);
}

bool Partie::jouerCoup(const Coup& coup) {
    Piece* piece = plateau_.pieceEn(coup.origine);
    if (!piece) return false;
    if (piece->getCouleur() != joueurActif().couleur) return false;

    auto coupsLegaux = piece->coupsPossibles(plateau_);
    if (std::find(coupsLegaux.begin(), coupsLegaux.end(), coup.destination) == coupsLegaux.end()) {
        return false;
    }

    // Capture éventuelle.
    Piece* pris = plateau_.pieceEn(coup.destination);
    if (pris && pris->getCouleur() == piece->getCouleur()) return false;

    plateau_.deplacer(coup.origine, coup.destination);
    piece->setPosition(coup.destination);

    if (estEnEchec(piece->getCouleur())) {
        // rollback : le joueur ne peut laisser son roi en échec.
        plateau_.deplacer(coup.destination, coup.origine);
        piece->setPosition(coup.origine);
        if (pris) plateau_.poser(coup.destination, pris);
        return false;
    }

    historique_.push_back(coup);

    // Élimination des joueurs matés (spécificité Yalta).
    for (Joueur& j : joueurs_) {
        if (!j.estElimine && estMat(j.couleur)) {
            j.estElimine = true;
        }
    }

    passerAuSuivant();
    return true;
}

void Partie::passerAuSuivant() {
    const int n = static_cast<int>(joueurs_.size());
    for (int i = 1; i <= n; ++i) {
        int k = (tourCourant_ + i) % n;
        if (!joueurs_[k].estElimine) {
            tourCourant_ = k;
            return;
        }
    }
}

bool Partie::estTerminee() const {
    int actifs = 0;
    for (const Joueur& j : joueurs_) if (!j.estElimine) ++actifs;
    return actifs <= 1;
}

} // namespace yalta
