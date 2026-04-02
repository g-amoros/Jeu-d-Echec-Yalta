#include "PieceFactory.h"

#include <stdexcept>

namespace yalta {

namespace {

// Les Strategy concrètes seront implémentées à la session suivante.
// En attendant, creer() construit la pièce sans stratégie : les appels à
// Piece::coupsPossibles lèveront donc une erreur si on les sollicite avant
// la session 5. C'est volontaire pour échouer bruyamment.
std::unique_ptr<IStrategieDeplacement> strategiePour(TypePiece /*type*/) {
    return nullptr;
}

void ajouterCamp(std::vector<std::unique_ptr<Piece>>& pieces, Couleur couleur, std::int8_t secteur) {
    // Rang 1 : pions du camp.
    for (char col = 'a'; col <= 'h'; ++col) {
        pieces.push_back(PieceFactory::creer(TypePiece::PION, couleur, Position{secteur, col, 2}));
    }
    // Rang 0 : pièces majeures dans l'ordre T C F D R F C T.
    const TypePiece ordre[8] = {
        TypePiece::TOUR, TypePiece::CAVALIER, TypePiece::FOU, TypePiece::REINE,
        TypePiece::ROI,  TypePiece::FOU,      TypePiece::CAVALIER, TypePiece::TOUR
    };
    char col = 'a';
    for (TypePiece t : ordre) {
        pieces.push_back(PieceFactory::creer(t, couleur, Position{secteur, col, 1}));
        ++col;
    }
}

} // namespace

std::unique_ptr<Piece> PieceFactory::creer(TypePiece type, Couleur couleur, Position pos) {
    if (!pos.estValide()) {
        throw std::invalid_argument("PieceFactory::creer : position invalide");
    }
    return std::make_unique<Piece>(couleur, type, pos, strategiePour(type));
}

std::vector<std::unique_ptr<Piece>> PieceFactory::dispositionInitiale() {
    std::vector<std::unique_ptr<Piece>> pieces;
    pieces.reserve(48);
    ajouterCamp(pieces, Couleur::BLANC, 0);
    ajouterCamp(pieces, Couleur::NOIR,  1);
    ajouterCamp(pieces, Couleur::ROUGE, 2);
    return pieces;
}

} // namespace yalta
