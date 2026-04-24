#pragma once

#include "Position.h"
#include "TypePiece.h"

namespace yalta {

enum class TypeCoup {
    NORMAL,           // coup classique sans règle spéciale
    ROQUE_PETIT,      // roque du cote roi (petit roque)
    ROQUE_GRAND,      // roque du cote dame (grand roque)
    PROMOTION,        // promotion d'un pion en arrivant sur la derniere rangee
    PRISE_EN_PASSANT  // prise speciale d'un pion "en passant"
};

/// Description d'un coup joué (ou à jouer) dans la partie.
struct Coup {
    Position  origine      {};
    Position  destination  {};
    TypeCoup  type         {TypeCoup::NORMAL};
    TypePiece piecePromue  {TypePiece::REINE}; // utilisé si type == PROMOTION
};

} // namespace yalta
