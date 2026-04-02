#pragma once

#include "Couleur.h"
#include "Position.h"
#include <vector>

namespace yalta {

class Plateau;

/**
 * @brief Interface Strategy pour le calcul des coups possibles d'une pièce.
 *
 * Une implémentation concrète existe par type de pièce (DeplacementPion, ...).
 * Cette abstraction permettra également d'injecter ultérieurement une stratégie
 * de décision IA (min-max / alpha-beta) sans modifier le moteur.
 */
class IStrategieDeplacement {
public:
    virtual ~IStrategieDeplacement() = default;

    /// Retourne les positions atteignables depuis @p origine par une pièce de
    /// couleur @p couleur, compte tenu de l'état actuel du @p plateau.
    /// Ne filtre pas les coups qui mettraient le roi en échec : ce contrôle
    /// est effectué par Partie avant validation.
    [[nodiscard]] virtual std::vector<Position> coupsPossibles(
        const Plateau& plateau,
        Position       origine,
        Couleur        couleur) const = 0;
};

} // namespace yalta
