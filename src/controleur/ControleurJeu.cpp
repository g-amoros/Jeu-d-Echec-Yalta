#include "ControleurJeu.h"

#include "ia/IAMinMax.h"
#include "modele/Coup.h"
#include "modele/Couleur.h"
#include "modele/Partie.h"
#include "modele/Piece.h"
#include "modele/TypePiece.h"
#include "vue/VuePlateau.h"

#include <QTimer>

#include <algorithm>
#include <vector>

namespace yalta {

ControleurJeu::ControleurJeu(Partie& partie, VuePlateau& vue, QObject* parent)
    : QObject(parent), partie_(partie), vue_(vue)
{
    QObject::connect(&vue_, &VuePlateau::caseCliquee,
                     this,  &ControleurJeu::onCaseCliquee);
}

void ControleurJeu::setJoueurIA(Couleur couleur, bool estIA) {
    if (estIA) {
        joueursIA_.insert(couleur);
    } else {
        joueursIA_.erase(couleur);
    }

    emit tourChange(messageTour());
    if (estIA && !partie_.estTerminee() && partie_.joueurActif().couleur == couleur) {
        QTimer::singleShot(10, this, &ControleurJeu::jouerTourIA);
    }
}

void ControleurJeu::setProfondeurIA(int profondeur) {
    profondeurIA_ = std::clamp(profondeur, 1, 5);
}

QString ControleurJeu::messageTour() const {
    if (partie_.estTerminee()) {
        return QStringLiteral("Fin de partie !");
    }

    QString nom;
    switch (partie_.joueurActif().couleur) {
        case Couleur::BLANC: nom = QStringLiteral("Blancs"); break;
        case Couleur::NOIR:  nom = QStringLiteral("Noirs");  break;
        case Couleur::ROUGE: nom = QStringLiteral("Rouges"); break;
    }

    const bool estIA = joueursIA_.count(partie_.joueurActif().couleur) > 0;
    return QStringLiteral("Au tour des %1%2")
        .arg(nom, estIA ? QStringLiteral(" (IA)") : QString());
}

void ControleurJeu::notifierChangementTour() {
    if (partie_.estTerminee()) {
        emit tourChange(QStringLiteral("Fin de partie !"));
        emit partieTerminee(QStringLiteral("La partie est terminée."));
    } else {
        emit tourChange(messageTour());
        if (joueursIA_.count(partie_.joueurActif().couleur) > 0) {
            QTimer::singleShot(100, this, &ControleurJeu::jouerTourIA);
        }
    }
}

void ControleurJeu::onCaseCliquee(Position pos) {
    if (partie_.estTerminee()) return;
    if (joueursIA_.count(partie_.joueurActif().couleur) > 0) return;

    if (!selection_) {
        Piece* piece = partie_.plateau().pieceEn(pos);
        if (!piece || piece->getCouleur() != partie_.joueurActif().couleur) return;

        selection_ = pos;
        const auto coups = partie_.coupsLegaux();
        std::vector<Position> destinations;
        for (const auto& coup : coups) {
            if (coup.origine == pos) destinations.push_back(coup.destination);
        }
        vue_.selectionner(pos);
        vue_.surbrillance(destinations);
        vue_.rafraichir();
        return;
    }

    const Coup coup{*selection_, pos, TypeCoup::NORMAL, TypePiece::REINE};
    selection_.reset();
    vue_.selectionner(std::nullopt);
    vue_.surbrillance({});

    const bool coupJoue = partie_.jouerCoup(coup);
    if (coupJoue) vue_.setDernierCoup(coup);
    vue_.rafraichir();

    if (coupJoue) notifierChangementTour();
}

void ControleurJeu::jouerTourIA() {
    if (partie_.estTerminee()) return;

    const Couleur actuel = partie_.joueurActif().couleur;
    if (joueursIA_.count(actuel) == 0) return;

    IAMinMax ia(profondeurIA_);
    const Coup coup = ia.meilleurCoup(partie_);
    if (!partie_.jouerCoup(coup)) return;

    vue_.selectionner(std::nullopt);
    vue_.surbrillance({});
    vue_.setDernierCoup(coup);
    vue_.rafraichir();

    notifierChangementTour();
}

void ControleurJeu::reinitialiser() {
    selection_.reset();
    vue_.selectionner(std::nullopt);
    vue_.surbrillance({});
    vue_.setDernierCoup(std::nullopt);
    vue_.rafraichir();
    emit tourChange(messageTour());

    if (!partie_.estTerminee() && joueursIA_.count(partie_.joueurActif().couleur) > 0) {
        QTimer::singleShot(100, this, &ControleurJeu::jouerTourIA);
    }
}

} // namespace yalta
