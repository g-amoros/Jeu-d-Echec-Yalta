#pragma once

#include "Couleur.h"
#include "IStrategieDeplacement.h"
#include "Position.h"
#include "TypePiece.h"
#include <memory>
#include <vector>

namespace yalta {

class Plateau;

/**
 * @brief Représente une pièce du jeu.
 *
 * La pièce ne connaît pas les règles de déplacement propres à son type : elle
 * les délègue à une IStrategieDeplacement injectée à la construction (pattern
 * Strategy). L'état stocké est minimal : couleur, type, position courante, et
 * un booléen « a déjà bougé » utile pour le roque et le premier pas du pion.
 */
class Piece {
public:
    Piece(Couleur couleur,
          TypePiece type,
          Position position,
          std::unique_ptr<IStrategieDeplacement> strategie)
        : couleur_(couleur)
        , type_(type)
        , position_(position)
        , strategie_(std::move(strategie)) {}

    virtual ~Piece() = default;

    [[nodiscard]] Couleur   getCouleur()  const noexcept { return couleur_; }
    [[nodiscard]] TypePiece getType()     const noexcept { return type_; }
    [[nodiscard]] Position  getPosition() const noexcept { return position_; }
    [[nodiscard]] bool      aDejaBouge()  const noexcept { return aDejaBouge_; }

    void setPosition(Position p) noexcept { position_ = p; aDejaBouge_ = true; }

    [[nodiscard]] std::vector<Position> coupsPossibles(const Plateau& plateau) const {
        return strategie_->coupsPossibles(plateau, position_, couleur_);
    }

private:
    Couleur                                couleur_;
    TypePiece                              type_;
    Position                               position_;
    bool                                   aDejaBouge_ {false};
    std::unique_ptr<IStrategieDeplacement> strategie_;
};

} // namespace yalta
