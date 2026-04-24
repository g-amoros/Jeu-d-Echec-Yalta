#pragma once

#include <QMainWindow>

class QLabel;
class QPushButton;
class QSpinBox;

namespace yalta {

class Partie;
class VuePlateau;
class ControleurJeu;

class FenetrePrincipale : public QMainWindow {
    Q_OBJECT

public:
    explicit FenetrePrincipale(QWidget* parent = nullptr);
    ~FenetrePrincipale() override;

private slots:
    void onTourChange(const QString& message);
    void onPartieTerminee(const QString& message);
    void onNouvelleParte();

private:
    QWidget* construirePanneau();

    Partie*        partie_        {nullptr};
    VuePlateau*    vuePlateau_    {nullptr};
    ControleurJeu* controleur_    {nullptr};

    QLabel*      lblTour_          {nullptr};
    QPushButton* btnBlancsIA_      {nullptr};
    QPushButton* btnNoirsIA_       {nullptr};
    QPushButton* btnRougesIA_      {nullptr};
    QSpinBox*    sbProfondeur_     {nullptr};
    QPushButton* btnNouvelleParte_ {nullptr};
};

} // namespace yalta
