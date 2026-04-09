#pragma once

#include "Coup.h"
#include "Joueur.h"
#include "Piece.h"
#include "Plateau.h"
#include <memory>
#include <vector>

namespace yalta {

/**
 * @brief Moteur d'une partie Yalta à 3 joueurs.
 *
 * Partie est propriétaire des pièces et du plateau. Elle expose les opérations
 * de haut niveau : démarrer une partie, jouer un coup légal, interroger les
 * états d'échec / mat / pat, éliminer un joueur maté, détecter une fin de
 * partie (il ne reste qu'un joueur actif ou aucun coup légal restant).
 */
class Partie {
public:
    Partie();

    [[nodiscard]] const Plateau& plateau() const noexcept { return plateau_; }

    [[nodiscard]] const Joueur& joueurActif() const noexcept { return joueurs_[tourCourant_]; }

    /// Tente de jouer @p coup. Retourne true si le coup a été appliqué.
    bool jouerCoup(const Coup& coup);

    [[nodiscard]] bool estEnEchec(Couleur c) const;
    [[nodiscard]] bool estMat(Couleur c) const;
    [[nodiscard]] bool estPat(Couleur c) const;

    /// Indique si la partie est terminée (≤ 1 joueur non éliminé).
    [[nodiscard]] bool estTerminee() const;

private:
    void passerAuSuivant();
    [[nodiscard]] Piece* trouverRoi(Couleur c) const;
    [[nodiscard]] bool aUnCoupLegal(Couleur c) const;

    Plateau                              plateau_;
    std::vector<std::unique_ptr<Piece>>  pieces_;
    std::vector<Joueur>                  joueurs_;
    std::vector<Coup>                    historique_;
    int                                  tourCourant_ {0};
};

} // namespace yalta
