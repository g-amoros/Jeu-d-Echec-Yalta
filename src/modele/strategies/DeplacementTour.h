#pragma once

#include "../IStrategieDeplacement.h"

namespace yalta {

/// Déplacement en ligne/colonne jusqu'à collision ou bord.
class DeplacementTour : public IStrategieDeplacement {
public:
    std::vector<Position> coupsPossibles(
        const Plateau& plateau,
        Position       origine,
        Couleur        couleur) const override;
};

} // namespace yalta
