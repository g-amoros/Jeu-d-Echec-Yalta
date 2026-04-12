#pragma once

#include "modele/Position.h"
#include <QObject>
#include <optional>

namespace yalta {

class Partie;
class VuePlateau;

/**
 * @brief Contrôleur MVC : relie la Vue Qt au Modèle Partie.
 *
 * Il maintient la sélection courante (première case cliquée) et, au second
 * clic, tente de jouer le coup correspondant via Partie::jouerCoup. Les
 * coups illégaux sont ignorés et la sélection est réinitialisée.
 */
class ControleurJeu : public QObject {
    Q_OBJECT

public:
    ControleurJeu(Partie& partie, VuePlateau& vue, QObject* parent = nullptr);

public slots:
    void onCaseCliquee(Position pos);

private:
    Partie&                 partie_;
    VuePlateau&             vue_;
    std::optional<Position> selection_;
};

} // namespace yalta
