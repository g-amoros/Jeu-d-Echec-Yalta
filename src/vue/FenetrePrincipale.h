#pragma once

#include <QMainWindow>

namespace yalta {

class Partie;
class VuePlateau;
class ControleurJeu;

/**
 * @brief Fenêtre principale Qt : contient la VuePlateau et une barre de statut.
 */
class FenetrePrincipale : public QMainWindow {
    Q_OBJECT

public:
    explicit FenetrePrincipale(QWidget* parent = nullptr);
    ~FenetrePrincipale() override;

private:
    Partie*        partie_     {nullptr};
    VuePlateau*    vuePlateau_ {nullptr};
    ControleurJeu* controleur_ {nullptr};
};

} // namespace yalta
