#pragma once

#include "ia/IAMinMax.h"
#include "modele/Couleur.h"
#include "modele/Position.h"
#include <QObject>
#include <QString>
#include <optional>
#include <set>

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

    void setJoueurIA(Couleur couleur, bool estIA);
    void setProfondeurIA(int profondeur);

signals:
    void tourChange(const QString& message);

public slots:
    void onCaseCliquee(Position pos);

private slots:
    void jouerTourIA();

private:
    [[nodiscard]] QString messageTour() const;

    Partie&                 partie_;
    VuePlateau&             vue_;
    std::optional<Position> selection_;
    std::set<Couleur>       joueursIA_;
    int                     profondeurIA_ {3};
};

} // namespace yalta
