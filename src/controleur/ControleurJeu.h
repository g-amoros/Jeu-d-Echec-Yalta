#pragma once

#include "modele/Couleur.h"
#include "modele/Position.h"
#include <QObject>
#include <QString>
#include <optional>
#include <set>

namespace yalta {

class Partie;
class VuePlateau;

class ControleurJeu : public QObject {
    Q_OBJECT

public:
    ControleurJeu(Partie& partie, VuePlateau& vue, QObject* parent = nullptr);

    void setJoueurIA(Couleur couleur, bool estIA);
    void setProfondeurIA(int profondeur);

    [[nodiscard]] QString messageTour() const;

signals:
    void tourChange(const QString& message);
    void partieTerminee(const QString& message);

public slots:
    void onCaseCliquee(Position pos);
    void reinitialiser();

private slots:
    void jouerTourIA();

private:
    void notifierChangementTour();
    [[nodiscard]] QString construireMessageFin() const;

    Partie&                 partie_;
    VuePlateau&             vue_;
    std::optional<Position> selection_;
    std::set<Couleur>       joueursIA_;
    int                     profondeurIA_ {3};
};

} // namespace yalta
