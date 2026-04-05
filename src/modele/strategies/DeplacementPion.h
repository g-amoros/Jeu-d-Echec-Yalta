#pragma once

#include "../IStrategieDeplacement.h"

namespace yalta {

/**
 * @brief Stratégie de déplacement d'un pion.
 *
 * Règles classiques adaptées Yalta :
 *  - avance d'une case vers l'avant de son camp (case vide) ;
 *  - avance de deux cases au premier coup ;
 *  - capture en diagonale avant ;
 *  - la prise en passant est gérée par Partie (contexte du coup précédent).
 */
class DeplacementPion : public IStrategieDeplacement {
public:
    std::vector<Position> coupsPossibles(
        const Plateau& plateau,
        Position       origine,
        Couleur        couleur) const override;
};

} // namespace yalta
