#include "DeplacementReine.h"

#include "../GeometriePlateau.h"
#include "../Piece.h"
#include "../Plateau.h"

namespace yalta {

std::vector<Position> DeplacementReine::coupsPossibles(
    const Plateau& plateau,
    Position       origine,
    Couleur        couleur) const
{
    std::vector<Position> coups;

    for (const auto& rayon : geometrie::rayonsArete(origine)) {
        for (Position position : rayon) {
            Piece* cible = plateau.pieceEn(position);
            if (!cible) {
                coups.push_back(position);
                continue;
            }
            if (cible->getCouleur() != couleur) {
                coups.push_back(position);
            }
            break;
        }
    }

    for (const auto& rayon : geometrie::rayonsCoin(origine)) {
        for (Position position : rayon) {
            Piece* cible = plateau.pieceEn(position);
            if (!cible) {
                coups.push_back(position);
                continue;
            }
            if (cible->getCouleur() != couleur) {
                coups.push_back(position);
            }
            break;
        }
    }

    return coups;
}

} // namespace yalta
