#include "Plateau.h"

namespace yalta {

Piece* Plateau::pieceEn(Position p) const noexcept {
    auto it = cases_.find(p);
    return (it != cases_.end()) ? it->second : nullptr;
}

void Plateau::poser(Position p, Piece* piece) {
    if (piece == nullptr) { cases_.erase(p); return; }
    cases_[p] = piece;
}

void Plateau::retirer(Position p) {
    cases_.erase(p);
}

void Plateau::deplacer(Position origine, Position destination) {
    auto it = cases_.find(origine);
    if (it == cases_.end()) return;
    Piece* piece = it->second;
    cases_.erase(it);
    cases_[destination] = piece;
}

} // namespace yalta
