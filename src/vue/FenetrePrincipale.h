#pragma once

#include <QMainWindow>

class QCheckBox;
class QSpinBox;

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
    QCheckBox*     cbBlancsIA_ {nullptr};
    QCheckBox*     cbNoirsIA_ {nullptr};
    QCheckBox*     cbRougesIA_ {nullptr};
    QSpinBox*      sbProfondeur_ {nullptr};
};

} // namespace yalta
