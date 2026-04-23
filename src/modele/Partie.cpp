#include "Partie.h"

#include "PieceFactory.h"

#include <algorithm>
#include <cstdlib>

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

Partie::Partie(const Partie& other)
    : joueurs_(other.joueurs_)
    , historique_(other.historique_)
    , tourCourant_(other.tourCourant_)
{
    for (const auto& p : other.pieces_)
        pieces_.push_back(p->clone());
    for (const auto& p : pieces_)
        plateau_.poser(p->getPosition(), p.get());
}

Partie& Partie::operator=(const Partie& other) {
    Partie tmp(other);
    std::swap(plateau_,     tmp.plateau_);
    std::swap(pieces_,      tmp.pieces_);
    std::swap(joueurs_,     tmp.joueurs_);
    std::swap(historique_,  tmp.historique_);
    std::swap(tourCourant_, tmp.tourCourant_);
    return *this;
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

std::vector<Coup> Partie::coupsLegaux() const {
    Couleur c = joueurActif().couleur;
    std::vector<Coup> legaux;
    for (const auto& p : pieces_) {
        if (p->getCouleur() != c) continue;
        for (Position dest : p->coupsPossibles(plateau_)) {
            TypeCoup t = TypeCoup::NORMAL;
            if (p->getType() == TypePiece::PION && dest.rang == 4)
                t = TypeCoup::PROMOTION;
            Coup coup{p->getPosition(), dest, t, TypePiece::REINE};
            Partie copie = *this;
            if (copie.jouerCoup(coup))
                legaux.push_back(coup);
        }
    }
    return legaux;
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

bool Partie::priseEnPassantPossible(Position origine, Position destination) const {
    if (historique_.empty()) return false;
    const Coup& prec = historique_.back();

    Piece* piecePrec = plateau_.pieceEn(prec.destination);
    if (!piecePrec || piecePrec->getType() != TypePiece::PION) return false;

    // Le pion adverse vient d'avancer de deux cases.
    if (std::abs(prec.destination.rang - prec.origine.rang) != 2) return false;

    Piece* pion = plateau_.pieceEn(origine);
    if (!pion || pion->getType() != TypePiece::PION) return false;

    // La destination doit être la case « sautée » par le pion adverse.
    Position passage{prec.origine.secteur,
                     prec.origine.colonne,
                     static_cast<std::int8_t>((prec.origine.rang + prec.destination.rang) / 2)};
    return destination == passage && origine.rang == prec.destination.rang;
}

bool Partie::promouvoir(Position pos, TypePiece nouveauType) {
    Piece* pion = plateau_.pieceEn(pos);
    if (!pion || pion->getType() != TypePiece::PION) return false;
    if (nouveauType == TypePiece::PION || nouveauType == TypePiece::ROI) return false;

    Couleur c = pion->getCouleur();
    // Retirer l'ancienne pièce du pool et du plateau.
    plateau_.retirer(pos);
    pieces_.erase(std::remove_if(pieces_.begin(), pieces_.end(),
        [&](const std::unique_ptr<Piece>& u) { return u.get() == pion; }),
        pieces_.end());

    // Créer la nouvelle pièce.
    pieces_.push_back(PieceFactory::creer(nouveauType, c, pos));
    plateau_.poser(pos, pieces_.back().get());
    return true;
}

bool Partie::tenterRoque(TypeCoup sens) {
    const Couleur c = joueurActif().couleur;
    Piece* roi = trouverRoi(c);
    if (!roi || roi->aDejaBouge() || estEnEchec(c)) return false;

    const int dcTour   = (sens == TypeCoup::ROQUE_PETIT) ? +3 : -4;
    const int dcMilieu = (sens == TypeCoup::ROQUE_PETIT) ? +1 : -1;
    const int dcRoi    = (sens == TypeCoup::ROQUE_PETIT) ? +2 : -2;

    Position posRoi  = roi->getPosition();
    Position posTour{posRoi.secteur,
                     static_cast<char>(posRoi.colonne + dcTour),
                     posRoi.rang};
    Piece* tour = plateau_.pieceEn(posTour);
    if (!tour || tour->getType() != TypePiece::TOUR || tour->aDejaBouge()) return false;

    // Cases entre le roi et la tour doivent être vides et non menacées.
    for (int step = 1; step <= std::abs(dcTour) - 1; ++step) {
        int dir = (dcTour > 0) ? +1 : -1;
        Position c1{posRoi.secteur,
                    static_cast<char>(posRoi.colonne + dir * step),
                    posRoi.rang};
        if (!plateau_.caseVide(c1)) return false;
        if (step <= 2 && plateau_.estMenacee(c1, c)) return false;
    }

    Position destRoi {posRoi.secteur, static_cast<char>(posRoi.colonne + dcRoi),    posRoi.rang};
    Position destTour{posRoi.secteur, static_cast<char>(posRoi.colonne + dcMilieu), posRoi.rang};

    plateau_.deplacer(posRoi,  destRoi);  roi->setPosition(destRoi);
    plateau_.deplacer(posTour, destTour); tour->setPosition(destTour);

    historique_.push_back(Coup{posRoi, destRoi, sens, TypePiece::REINE});
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
