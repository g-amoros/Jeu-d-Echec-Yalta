#include "DeplacementCavalier.h"

#include "../GeometriePlateau.h"
#include "../Piece.h"
#include "../Plateau.h"

namespace yalta {

std::vector<Position> DeplacementCavalier::coupsPossibles(
    const Plateau& plateau,
    Position       origine,
    Couleur        couleur) const
{
    std::vector<Position> coups;

    for (const Position& position : geometrie::sautsCavalier(origine)) {
        Piece* cible = plateau.pieceEn(position);
        if (!cible || cible->getCouleur() != couleur) {
            coups.push_back(position);
        }
    }
    return coups;
}

} // namespace yalta
