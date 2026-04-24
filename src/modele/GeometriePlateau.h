#pragma once

#include "Position.h"
#include <vector>

namespace yalta::geometrie {

[[nodiscard]] const std::vector<Position>& voisinsArete(Position position);
[[nodiscard]] const std::vector<Position>& voisinsCoin(Position position);

[[nodiscard]] std::vector<std::vector<Position>> rayonsArete(Position position);
[[nodiscard]] std::vector<std::vector<Position>> rayonsCoin(Position position);
[[nodiscard]] std::vector<Position> sautsCavalier(Position position);

} // namespace yalta::geometrie
