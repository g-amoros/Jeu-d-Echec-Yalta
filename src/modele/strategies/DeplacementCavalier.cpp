#include "DeplacementCavalier.h"

#include "../Piece.h"
#include "../Plateau.h"

namespace yalta {

std::vector<Position> DeplacementCavalier::coupsPossibles(
    const Plateau& plateau,
    Position       origine,
    Couleur        couleur) const
{
    std::vector<Position> coups;
    const int sauts[8][2] = {
        {+2, +1}, {+2, -1}, {-2, +1}, {-2, -1},
        {+1, +2}, {+1, -2}, {-1, +2}, {-1, -2}
    };

    for (const auto& s : sauts) {
        Position p{origine.secteur,
                   static_cast<char>(origine.colonne + s[0]),
                   static_cast<std::int8_t>(origine.rang + s[1])};
        if (!p.estValide()) continue;
        Piece* cible = plateau.pieceEn(p);
        if (!cible || cible->getCouleur() != couleur) {
            coups.push_back(p);
        }
    }
    return coups;
}

} // namespace yalta
