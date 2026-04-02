#pragma once

#include <cstdint>
#include <functional>

/**
 * @file Position.h
 * @brief Coordonnée d'une case sur le plateau Yalta.
 *
 * Le plateau Yalta est composé de trois secteurs de 32 cases chacun, partagés
 * autour d'un centre hexagonal. Une Position est donc identifiée par :
 *   - un secteur (0, 1 ou 2) correspondant au camp de départ ;
 *   - une colonne (lettre 'a'..'h') ;
 *   - un rang (1..4) à l'intérieur du secteur.
 */

namespace yalta {

struct Position {
    std::int8_t secteur {0};
    char        colonne {'a'};
    std::int8_t rang    {1};

    [[nodiscard]] constexpr bool estValide() const noexcept {
        return secteur >= 0 && secteur <= 2
            && colonne >= 'a' && colonne <= 'h'
            && rang >= 1 && rang <= 4;
    }

    friend constexpr bool operator==(const Position& a, const Position& b) noexcept {
        return a.secteur == b.secteur && a.colonne == b.colonne && a.rang == b.rang;
    }

    friend constexpr bool operator!=(const Position& a, const Position& b) noexcept {
        return !(a == b);
    }
};

} // namespace yalta

namespace std {
template <>
struct hash<yalta::Position> {
    std::size_t operator()(const yalta::Position& p) const noexcept {
        return static_cast<std::size_t>(p.secteur) * 131
             + static_cast<std::size_t>(p.colonne) * 17
             + static_cast<std::size_t>(p.rang);
    }
};
} // namespace std
