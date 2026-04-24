#pragma once

#include "modele/Coup.h"
#include "modele/Couleur.h"
#include "modele/Partie.h"

namespace yalta {

/**
 * @brief IA min-max paranoïaque avec élagage alpha-bêta pour Yalta à 3 joueurs.
 *
 * Modèle paranoïaque : le joueur actif maximise son score matériel, les deux
 * autres forment une coalition minimisante. L'élagage alpha-bêta réduit la
 * complexité de O(b^d) à O(b^(d/2)) avec un bon tri des coups.
 */
class IAMinMax {
public:
    explicit IAMinMax(int profondeur = 3);

    [[nodiscard]] Coup meilleurCoup(const Partie& partie) const;

private:
    static int  evaluer(const Partie& partie, Couleur joueurIA);
    int minmax(Partie partie, int profondeur, int alpha, int beta,
               bool maximisant, Couleur joueurIA) const;

    int profondeur_ {3};
};

} // namespace yalta
