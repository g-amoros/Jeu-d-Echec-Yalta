#pragma once

#include "Couleur.h"
#include <optional>

namespace yalta {

enum class RaisonElimination { MAT, PAT };

/// Joueur d'une partie Yalta (état minimal : couleur + statut).
struct Joueur {
    Couleur couleur;
    bool    estElimine {false};
    std::optional<RaisonElimination> raisonElimination {};
};

} // namespace yalta
