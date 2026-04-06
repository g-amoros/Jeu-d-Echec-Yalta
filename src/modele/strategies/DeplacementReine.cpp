#include "DeplacementReine.h"

#include "../Piece.h"
#include "../Plateau.h"

namespace yalta {

std::vector<Position> DeplacementReine::coupsPossibles(
    const Plateau& plateau,
    Position       origine,
    Couleur        couleur) const
{
    std::vector<Position> coups;
    const int directions[8][2] = {
        {+1, 0},  {-1, 0},  {0, +1},  {0, -1},
        {+1, +1}, {+1, -1}, {-1, +1}, {-1, -1}
    };

    for (const auto& d : directions) {
        for (int k = 1; k < 8; ++k) {
            Position p{origine.secteur,
                       static_cast<char>(origine.colonne + d[0] * k),
                       static_cast<std::int8_t>(origine.rang + d[1] * k)};
            if (!p.estValide()) break;
            Piece* cible = plateau.pieceEn(p);
            if (!cible) {
                coups.push_back(p);
            } else {
                if (cible->getCouleur() != couleur) coups.push_back(p);
                break;
            }
        }
    }
    return coups;
}

} // namespace yalta
