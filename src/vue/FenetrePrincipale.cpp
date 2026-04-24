#include "FenetrePrincipale.h"

#include "VuePlateau.h"
#include "controleur/ControleurJeu.h"
#include "modele/Couleur.h"
#include "modele/Partie.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QStatusBar>
#include <QVBoxLayout>

namespace yalta {

// ── Palette Catppuccin Mocha adaptée ──────────────────────────────────────

static constexpr const char* THEME_QSS = R"(
QMainWindow {
    background-color: #1e1e2e;
}
QWidget#central {
    background-color: #1e1e2e;
}

/* ── Sidebar ── */
QWidget#panneau {
    background-color: #181825;
    border-left: 1px solid #313244;
}

/* ── Titre ── */
QLabel#titre {
    color: #cdd6f4;
    font-size: 22px;
    font-weight: bold;
    letter-spacing: 6px;
    padding-top: 4px;
}
QLabel#sousTitre {
    color: #585b70;
    font-size: 10px;
    letter-spacing: 2px;
    padding-bottom: 4px;
}

/* ── Indicateur de tour ── */
QLabel#lblTour {
    color: #cdd6f4;
    font-size: 13px;
    font-weight: 600;
    padding: 9px 12px;
    border-radius: 7px;
    background-color: #2a2a3d;
    border: 1px solid #45475a;
}

/* ── Séparateurs ── */
QFrame#sep {
    color: #313244;
    max-height: 1px;
    background-color: #313244;
}

/* ── En-têtes de section ── */
QLabel#sectionTitre {
    color: #585b70;
    font-size: 10px;
    letter-spacing: 2px;
    font-weight: bold;
    margin-top: 4px;
}

/* ── Cartes joueur ── */
QWidget#carteJoueur {
    background-color: #24273a;
    border-radius: 7px;
    border: 1px solid #363a4f;
}

QLabel#nomJoueur {
    color: #b8c0e0;
    font-size: 12px;
}

/* ── Boutons IA (toggle) ── */
QPushButton#btnIA {
    background-color: #363a4f;
    border: 1px solid #494d64;
    border-radius: 5px;
    padding: 3px 10px;
    color: #b8c0e0;
    font-size: 11px;
    min-width: 58px;
}
QPushButton#btnIA:hover {
    background-color: #494d64;
    color: #cdd6f4;
}
QPushButton#btnIA:checked {
    background-color: #8aadf4;
    border-color: #8aadf4;
    color: #1e1e2e;
    font-weight: bold;
}

/* ── SpinBox profondeur ── */
QSpinBox {
    background-color: #363a4f;
    border: 1px solid #494d64;
    border-radius: 5px;
    padding: 4px 6px;
    color: #cdd6f4;
    min-width: 50px;
}
QSpinBox::up-button, QSpinBox::down-button {
    background-color: #494d64;
    border: none;
    width: 18px;
    border-radius: 3px;
}
QSpinBox::up-button:hover, QSpinBox::down-button:hover {
    background-color: #5b5f7b;
}
QLabel#lblProfondeur {
    color: #b8c0e0;
    font-size: 12px;
}

/* ── Bouton Nouvelle Partie ── */
QPushButton#btnNouvelleParte {
    background-color: #a6da95;
    color: #1e1e2e;
    font-size: 13px;
    font-weight: bold;
    border: none;
    border-radius: 8px;
    padding: 11px 0;
}
QPushButton#btnNouvelleParte:hover {
    background-color: #91d7b5;
}
QPushButton#btnNouvelleParte:pressed {
    background-color: #7dc4ac;
}

/* ── Barre de statut ── */
QStatusBar {
    background-color: #11111b;
    color: #7f849c;
    font-size: 12px;
    padding: 2px 10px;
    border-top: 1px solid #313244;
}
QStatusBar::item { border: none; }
)";

// ── Construction ──────────────────────────────────────────────────────────

FenetrePrincipale::FenetrePrincipale(QWidget* parent)
    : QMainWindow(parent)
{
    partie_     = new Partie();
    vuePlateau_ = new VuePlateau(*partie_, this);
    controleur_ = new ControleurJeu(*partie_, *vuePlateau_, this);

    auto* central = new QWidget(this);
    central->setObjectName(QStringLiteral("central"));

    auto* mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(vuePlateau_, 1);
    mainLayout->addWidget(construirePanneau(), 0);

    setCentralWidget(central);
    setStyleSheet(QLatin1String(THEME_QSS));

    lblTour_->setText(QStringLiteral("Au tour des Blancs"));
    statusBar()->showMessage(QStringLiteral("Yalta  ·  Au tour des Blancs"));

    // Connexions signaux
    QObject::connect(controleur_, &ControleurJeu::tourChange,
                     this, &FenetrePrincipale::onTourChange);
    QObject::connect(controleur_, &ControleurJeu::partieTerminee,
                     this, &FenetrePrincipale::onPartieTerminee);
    QObject::connect(btnNouvelleParte_, &QPushButton::clicked,
                     this, &FenetrePrincipale::onNouvelleParte);

    // Boutons IA
    auto connecterIA = [this](QPushButton* btn, Couleur couleur) {
        QObject::connect(btn, &QPushButton::toggled, this,
                         [this, btn, couleur](bool active) {
            btn->setText(active ? QStringLiteral("IA") : QStringLiteral("Humain"));
            controleur_->setJoueurIA(couleur, active);
        });
    };
    connecterIA(btnBlancsIA_,  Couleur::BLANC);
    connecterIA(btnNoirsIA_,   Couleur::NOIR);
    connecterIA(btnRougesIA_,  Couleur::ROUGE);

    QObject::connect(sbProfondeur_, QOverload<int>::of(&QSpinBox::valueChanged),
                     controleur_, &ControleurJeu::setProfondeurIA);

    resize(1080, 760);
    setWindowTitle(QStringLiteral("Jeu d'Échec Yalta"));
}

// ── Panneau latéral ───────────────────────────────────────────────────────

QWidget* FenetrePrincipale::construirePanneau() {
    auto* panneau = new QWidget(this);
    panneau->setObjectName(QStringLiteral("panneau"));
    panneau->setFixedWidth(215);

    auto* vl = new QVBoxLayout(panneau);
    vl->setContentsMargins(14, 18, 14, 18);
    vl->setSpacing(8);

    // ── Titre
    auto* lblTitre = new QLabel(QStringLiteral("YALTA"), panneau);
    lblTitre->setObjectName(QStringLiteral("titre"));
    lblTitre->setAlignment(Qt::AlignCenter);
    vl->addWidget(lblTitre);

    auto* lblSub = new QLabel(QStringLiteral("ÉCHECS À 3 JOUEURS"), panneau);
    lblSub->setObjectName(QStringLiteral("sousTitre"));
    lblSub->setAlignment(Qt::AlignCenter);
    vl->addWidget(lblSub);

    auto ajouterSep = [&]() {
        auto* sep = new QFrame(panneau);
        sep->setObjectName(QStringLiteral("sep"));
        sep->setFrameShape(QFrame::HLine);
        vl->addWidget(sep);
    };

    ajouterSep();

    // ── Tour actuel
    lblTour_ = new QLabel(panneau);
    lblTour_->setObjectName(QStringLiteral("lblTour"));
    lblTour_->setAlignment(Qt::AlignCenter);
    lblTour_->setWordWrap(true);
    vl->addWidget(lblTour_);

    ajouterSep();

    // ── Section joueurs
    auto* lblJoueurs = new QLabel(QStringLiteral("JOUEURS"), panneau);
    lblJoueurs->setObjectName(QStringLiteral("sectionTitre"));
    vl->addWidget(lblJoueurs);

    struct InfoJoueur {
        QString     nom;
        QString     cssColor;
        QPushButton** btn;
    };

    const std::array<InfoJoueur, 3> joueurs {{
        { QStringLiteral("Blancs"), QStringLiteral("#e2e2e2"), &btnBlancsIA_ },
        { QStringLiteral("Noirs"),  QStringLiteral("#777777"), &btnNoirsIA_  },
        { QStringLiteral("Rouges"), QStringLiteral("#ed4b4b"), &btnRougesIA_ },
    }};

    for (const auto& info : joueurs) {
        auto* carte = new QWidget(panneau);
        carte->setObjectName(QStringLiteral("carteJoueur"));
        carte->setAttribute(Qt::WA_StyledBackground, true);

        auto* hl = new QHBoxLayout(carte);
        hl->setContentsMargins(10, 7, 10, 7);
        hl->setSpacing(8);

        // Pastille colorée
        auto* pastille = new QLabel(panneau);
        pastille->setFixedSize(11, 11);
        pastille->setStyleSheet(QStringLiteral(
            "background-color: %1; border-radius: 5px;").arg(info.cssColor));
        hl->addWidget(pastille);

        // Nom du joueur
        auto* nomLbl = new QLabel(info.nom, panneau);
        nomLbl->setObjectName(QStringLiteral("nomJoueur"));
        hl->addWidget(nomLbl, 1);

        // Toggle Humain/IA
        auto* btn = new QPushButton(QStringLiteral("Humain"), panneau);
        btn->setObjectName(QStringLiteral("btnIA"));
        btn->setCheckable(true);
        *info.btn = btn;
        hl->addWidget(btn);

        vl->addWidget(carte);
    }

    ajouterSep();

    // ── Profondeur IA
    auto* lblSection2 = new QLabel(QStringLiteral("NIVEAU IA"), panneau);
    lblSection2->setObjectName(QStringLiteral("sectionTitre"));
    vl->addWidget(lblSection2);

    auto* depthRow = new QWidget(panneau);
    auto* depthLayout = new QHBoxLayout(depthRow);
    depthLayout->setContentsMargins(0, 0, 0, 0);
    depthLayout->setSpacing(8);

    auto* lblProf = new QLabel(QStringLiteral("Profondeur"), panneau);
    lblProf->setObjectName(QStringLiteral("lblProfondeur"));
    sbProfondeur_ = new QSpinBox(panneau);
    sbProfondeur_->setRange(1, 5);
    sbProfondeur_->setValue(3);

    depthLayout->addWidget(lblProf, 1);
    depthLayout->addWidget(sbProfondeur_);
    vl->addWidget(depthRow);

    ajouterSep();

    vl->addStretch();

    // ── Bouton nouvelle partie
    btnNouvelleParte_ = new QPushButton(QStringLiteral("Nouvelle Partie"), panneau);
    btnNouvelleParte_->setObjectName(QStringLiteral("btnNouvelleParte"));
    vl->addWidget(btnNouvelleParte_);

    return panneau;
}

// ── Destructor ────────────────────────────────────────────────────────────

FenetrePrincipale::~FenetrePrincipale() {
    delete partie_;
}

// ── Slots ─────────────────────────────────────────────────────────────────

void FenetrePrincipale::onTourChange(const QString& message) {
    lblTour_->setText(message);
    // Remettre le style normal si une partie précédente avait affiché "Fin"
    lblTour_->setStyleSheet(QString());
    statusBar()->showMessage(QStringLiteral("Yalta  ·  ") + message);
}

void FenetrePrincipale::onPartieTerminee(const QString& message) {
    lblTour_->setText(message);
    lblTour_->setWordWrap(true);

    const bool estVictoire = message.startsWith(QStringLiteral("Victoire"));
    if (estVictoire) {
        lblTour_->setStyleSheet(QStringLiteral(
            "color: #f5c542;"
            "font-size: 12px; font-weight: bold;"
            "padding: 9px 12px; border-radius: 7px;"
            "background-color: #352b08;"
            "border: 1px solid #f5c542;"
        ));
    } else {
        lblTour_->setStyleSheet(QStringLiteral(
            "color: #a6adc8;"
            "font-size: 12px; font-weight: bold;"
            "padding: 9px 12px; border-radius: 7px;"
            "background-color: #252535;"
            "border: 1px solid #585b70;"
        ));
    }
    statusBar()->showMessage(QStringLiteral("Yalta  ·  ") + message.section('\n', 0, 0));
}

void FenetrePrincipale::onNouvelleParte() {
    *partie_ = Partie();
    lblTour_->setStyleSheet(QString());
    controleur_->reinitialiser();
}

} // namespace yalta
