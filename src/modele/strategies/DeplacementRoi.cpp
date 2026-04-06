#include "DeplacementRoi.h"

#include "../Piece.h"
#include "../Plateau.h"

namespace yalta {

std::vector<Position> DeplacementRoi::coupsPossibles(
    const Plateau& plateau,
    Position       origine,
    Couleur        couleur) const
{
    std::vector<Position> coups;
    for (int dc = -1; dc <= 1; ++dc) {
        for (int dr = -1; dr <= 1; ++dr) {
            if (dc == 0 && dr == 0) continue;
            Position p{origine.secteur,
                       static_cast<char>(origine.colonne + dc),
                       static_cast<std::int8_t>(origine.rang + dr)};
            if (!p.estValide()) continue;
            Piece* cible = plateau.pieceEn(p);
            if (!cible || cible->getCouleur() != couleur) {
                coups.push_back(p);
            }
        }
    }
    return coups;
}

} // namespace yalta
