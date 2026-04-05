#pragma once

#include "../IStrategieDeplacement.h"

namespace yalta {

/// Déplacement en L (2+1) sans prise en compte des pièces intermédiaires.
class DeplacementCavalier : public IStrategieDeplacement {
public:
    std::vector<Position> coupsPossibles(
        const Plateau& plateau,
        Position       origine,
        Couleur        couleur) const override;
};

} // namespace yalta
