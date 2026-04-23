#include "Piece.h"
#include "PieceFactory.h"

namespace yalta {

std::unique_ptr<Piece> Piece::clone() const {
    auto copie = PieceFactory::creer(type_, couleur_, position_);
    if (aDejaBouge_) copie->marquerDeplace();
    return copie;
}

} // namespace yalta
