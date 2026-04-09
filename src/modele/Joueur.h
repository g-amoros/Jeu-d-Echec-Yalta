#pragma once

#include "Couleur.h"

namespace yalta {

/// Joueur d'une partie Yalta (état minimal : couleur + statut).
struct Joueur {
    Couleur couleur;
    bool    estElimine {false};
};

} // namespace yalta
