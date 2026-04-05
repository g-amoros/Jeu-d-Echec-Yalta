#pragma once

#include "../IStrategieDeplacement.h"

namespace yalta {

/// Déplacement diagonal jusqu'à collision ou bord.
class DeplacementFou : public IStrategieDeplacement {
public:
    std::vector<Position> coupsPossibles(
        const Plateau& plateau,
        Position       origine,
        Couleur        couleur) const override;
};

} // namespace yalta
