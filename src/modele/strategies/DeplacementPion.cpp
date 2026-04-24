#include "DeplacementPion.h"

#include "../Piece.h"
#include "../Plateau.h"

namespace yalta {

namespace {
// Sens de progression d'un pion selon le camp (rang qui augmente ou diminue
// dans son secteur d'origine). Simplification : chaque pion avance en
// augmentant son rang à l'intérieur de son secteur de départ.
int sensPour(Couleur) { return +1; }

Position captureInterSecteur(Position origine) {
    if (origine.colonne <= 'd') {
        return {
            static_cast<std::int8_t>((origine.secteur + 2) % 3),
            static_cast<char>('h' - (origine.colonne - 'a')),
            4
        };
    }

    return {
        static_cast<std::int8_t>((origine.secteur + 1) % 3),
        static_cast<char>('d' - (origine.colonne - 'e')),
        4
    };
}
} // namespace

std::vector<Position> DeplacementPion::coupsPossibles(
    const Plateau& plateau,
    Position       origine,
    Couleur        couleur) const
{
    std::vector<Position> coups;
    const int ds = sensPour(couleur);

    // Avance simple.
    Position devant{origine.secteur, origine.colonne, static_cast<std::int8_t>(origine.rang + ds)};
    if (devant.estValide() && plateau.caseVide(devant)) {
        coups.push_back(devant);
        // Avance double depuis le rang 2.
        if (origine.rang == 2) {
            Position devant2{origine.secteur, origine.colonne, static_cast<std::int8_t>(origine.rang + 2 * ds)};
            if (devant2.estValide() && plateau.caseVide(devant2)) {
                coups.push_back(devant2);
            }
        }
    }

    // Captures diagonales.
    for (int dc : {-1, +1}) {
        Position diag{origine.secteur,
                      static_cast<char>(origine.colonne + dc),
                      static_cast<std::int8_t>(origine.rang + ds)};
        if (!diag.estValide()) continue;
        Piece* cible = plateau.pieceEn(diag);
        if (cible && cible->getCouleur() != couleur) {
            coups.push_back(diag);
        }
    }

    // Au centre du plateau, le pion peut capturer la case adjacente située
    // dans le secteur voisin qu'il affronte.
    if (origine.rang == 4) {
        const Position cibleCentre = captureInterSecteur(origine);
        Piece* cible = plateau.pieceEn(cibleCentre);
        if (cible && cible->getCouleur() != couleur) {
            coups.push_back(cibleCentre);
        }
    }

    return coups;
}

} // namespace yalta
