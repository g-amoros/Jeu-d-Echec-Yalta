#pragma once

#include "Couleur.h"
#include "Position.h"
#include <unordered_map>

namespace yalta {

class Piece;

/**
 * @brief Plateau Yalta (96 cases réparties en 3 secteurs).
 *
 * Cette classe est un stockage clé/valeur Position → Piece*. Elle ne possède
 * pas les pièces (la propriété appartient à Partie).
 */
class Plateau {
public:
    /// Retourne la pièce en @p p ou nullptr si la case est vide.
    [[nodiscard]] Piece* pieceEn(Position p) const noexcept;

    /// Dépose une pièce sur une case (écrase ce qui s'y trouve).
    void poser(Position p, Piece* piece);

    /// Retire la pièce de @p p (ne la détruit pas).
    void retirer(Position p);

    /// Déplace la pièce de @p origine vers @p destination.
    void deplacer(Position origine, Position destination);

    [[nodiscard]] bool caseVide(Position p) const noexcept { return pieceEn(p) == nullptr; }

    /// Itération sur toutes les pièces présentes.
    [[nodiscard]] const std::unordered_map<Position, Piece*>& cases() const noexcept { return cases_; }

private:
    std::unordered_map<Position, Piece*> cases_;
};

} // namespace yalta
