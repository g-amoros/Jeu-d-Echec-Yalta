#include "FenetrePrincipale.h"

#include "VuePlateau.h"
#include "controleur/ControleurJeu.h"
#include "modele/Couleur.h"
#include "modele/Partie.h"

#include <QCheckBox>
#include <QLabel>
#include <QStatusBar>
#include <QSpinBox>
#include <QToolBar>

namespace yalta {

FenetrePrincipale::FenetrePrincipale(QWidget* parent)
    : QMainWindow(parent)
{
    partie_     = new Partie();
    vuePlateau_ = new VuePlateau(*partie_, this);
    controleur_ = new ControleurJeu(*partie_, *vuePlateau_, this);

    auto* toolBar = addToolBar(QStringLiteral("IA"));
    toolBar->setMovable(false);

    cbBlancsIA_ = new QCheckBox(QStringLiteral("Blancs IA"), this);
    cbNoirsIA_ = new QCheckBox(QStringLiteral("Noirs IA"), this);
    cbRougesIA_ = new QCheckBox(QStringLiteral("Rouges IA"), this);
    sbProfondeur_ = new QSpinBox(this);
    sbProfondeur_->setRange(1, 5);
    sbProfondeur_->setValue(3);

    toolBar->addWidget(cbBlancsIA_);
    toolBar->addWidget(cbNoirsIA_);
    toolBar->addWidget(cbRougesIA_);
    toolBar->addSeparator();
    toolBar->addWidget(new QLabel(QStringLiteral("Profondeur"), this));
    toolBar->addWidget(sbProfondeur_);

    setCentralWidget(vuePlateau_);
    QObject::connect(controleur_, &ControleurJeu::tourChange,
                     this, [this](const QString& message) {
                         statusBar()->showMessage(message);
                     });
    QObject::connect(cbBlancsIA_, &QCheckBox::toggled, this, [this](bool active) {
        controleur_->setJoueurIA(Couleur::BLANC, active);
    });
    QObject::connect(cbNoirsIA_, &QCheckBox::toggled, this, [this](bool active) {
        controleur_->setJoueurIA(Couleur::NOIR, active);
    });
    QObject::connect(cbRougesIA_, &QCheckBox::toggled, this, [this](bool active) {
        controleur_->setJoueurIA(Couleur::ROUGE, active);
    });
    QObject::connect(sbProfondeur_, QOverload<int>::of(&QSpinBox::valueChanged),
                     controleur_, &ControleurJeu::setProfondeurIA);

    statusBar()->showMessage(QStringLiteral("Yalta — au tour des Blancs"));
    resize(900, 700);
    setWindowTitle(QStringLiteral("Jeu d'Échec Yalta"));
}

FenetrePrincipale::~FenetrePrincipale() {
    delete partie_;
}

} // namespace yalta
