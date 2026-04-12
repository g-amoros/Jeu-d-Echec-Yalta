#include "ControleurJeu.h"

#include "modele/Partie.h"
#include "modele/Piece.h"
#include "vue/VuePlateau.h"

namespace yalta {

ControleurJeu::ControleurJeu(Partie& partie, VuePlateau& vue, QObject* parent)
    : QObject(parent), partie_(partie), vue_(vue)
{
    QObject::connect(&vue_, &VuePlateau::caseCliquee,
                     this,  &ControleurJeu::onCaseCliquee);
}

void ControleurJeu::onCaseCliquee(Position pos) {
    if (!selection_) {
        // Première sélection : on n'accepte que ses propres pièces.
        Piece* piece = partie_.plateau().pieceEn(pos);
        if (!piece || piece->getCouleur() != partie_.joueurActif().couleur) return;
        selection_ = pos;
        vue_.surbrillance(piece->coupsPossibles(partie_.plateau()));
        return;
    }

    // Second clic : tentative de coup.
    Coup c{*selection_, pos, TypeCoup::NORMAL, TypePiece::REINE};
    partie_.jouerCoup(c);
    selection_.reset();
    vue_.rafraichir();
}

} // namespace yalta
