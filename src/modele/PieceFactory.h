#pragma once

#include "Couleur.h"
#include "Piece.h"
#include "Position.h"
#include "TypePiece.h"
#include <memory>
#include <vector>

namespace yalta {

/**
 * @brief Factory des pièces du jeu (pattern Factory).
 *
 * Centralise l'instanciation d'une pièce : choix du bon type, injection de la
 * Strategy de déplacement correspondante, et production de la disposition
 * initiale Yalta (48 pièces réparties sur les 3 camps).
 */
class PieceFactory {
public:
    /// Crée une pièce du type et de la couleur demandés, positionnée en @p pos.
    static std::unique_ptr<Piece> creer(TypePiece type, Couleur couleur, Position pos);

    /// Produit les 48 pièces des trois camps, prêtes à être posées sur le plateau.
    static std::vector<std::unique_ptr<Piece>> dispositionInitiale();
};

} // namespace yalta
