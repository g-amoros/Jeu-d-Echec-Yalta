#include "FenetrePrincipale.h"

#include "VuePlateau.h"
#include "controleur/ControleurJeu.h"
#include "modele/Partie.h"

#include <QStatusBar>

namespace yalta {

FenetrePrincipale::FenetrePrincipale(QWidget* parent)
    : QMainWindow(parent)
{
    partie_     = new Partie();
    vuePlateau_ = new VuePlateau(*partie_, this);
    controleur_ = new ControleurJeu(*partie_, *vuePlateau_);

    setCentralWidget(vuePlateau_);
    statusBar()->showMessage(QStringLiteral("Yalta — au tour des Blancs"));
    resize(900, 700);
    setWindowTitle(QStringLiteral("Jeu d'Échec Yalta"));
}

FenetrePrincipale::~FenetrePrincipale() {
    delete controleur_;
    delete partie_;
}

} // namespace yalta
