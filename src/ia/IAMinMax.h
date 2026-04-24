#pragma once

#include "modele/Coup.h"
#include "modele/Couleur.h"
#include "modele/Partie.h"

namespace yalta {

/**
 * @brief IA min-max paranoïaque pour Yalta à 3 joueurs.
 *
 * Le joueur actif évalué maximise son score matériel, tandis que les deux
 * autres joueurs sont traités comme une coalition minimisante.
 */
class IAMinMax {
public:
    explicit IAMinMax(int profondeur = 3);

    [[nodiscard]] Coup meilleurCoup(const Partie& partie) const;

private:
    static int evaluer(const Partie& partie, Couleur joueurIA);
    int minmax(Partie partie, int profondeur, bool maximisant, Couleur joueurIA) const;

    int profondeur_ {3};
};

} // namespace yalta
