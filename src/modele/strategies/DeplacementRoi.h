#pragma once

#include "../IStrategieDeplacement.h"

namespace yalta {

/**
 * @brief Déplacement d'une case dans les 8 directions.
 *
 * Le roque n'est pas produit par cette Strategy : il est géré au niveau de
 * Partie, qui connaît l'état d'« a déjà bougé » du roi et de la tour, ainsi
 * que l'absence d'échec sur les cases traversées.
 */
class DeplacementRoi : public IStrategieDeplacement {
public:
    std::vector<Position> coupsPossibles(
        const Plateau& plateau,
        Position       origine,
        Couleur        couleur) const override;
};

} // namespace yalta
