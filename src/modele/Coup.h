#pragma once

#include "Position.h"
#include "TypePiece.h"

namespace yalta {

enum class TypeCoup {
    NORMAL,
    ROQUE_PETIT,
    ROQUE_GRAND,
    PROMOTION,
    PRISE_EN_PASSANT
};

/// Description d'un coup joué (ou à jouer) dans la partie.
struct Coup {
    Position  origine      {};
    Position  destination  {};
    TypeCoup  type         {TypeCoup::NORMAL};
    TypePiece piecePromue  {TypePiece::REINE}; // utilisé si type == PROMOTION
};

} // namespace yalta
