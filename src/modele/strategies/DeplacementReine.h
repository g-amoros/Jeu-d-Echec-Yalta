#pragma once

#include "../IStrategieDeplacement.h"

namespace yalta {

/// Déplacement combiné tour + fou (8 directions, portée illimitée).
class DeplacementReine : public IStrategieDeplacement {
public:
    std::vector<Position> coupsPossibles(
        const Plateau& plateau,
        Position       origine,
        Couleur        couleur) const override;
};

} // namespace yalta
