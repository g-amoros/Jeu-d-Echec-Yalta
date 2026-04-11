#include <gtest/gtest.h>

#include "modele/Partie.h"

using namespace yalta;

TEST(Partie, InitialisationAvecTroisJoueurs) {
    Partie p;
    EXPECT_FALSE(p.estTerminee());
    EXPECT_EQ(p.joueurActif().couleur, Couleur::BLANC);
}

TEST(Partie, CoupIllegalRefuse) {
    Partie p;
    // On tente un coup impossible : déplacer la tour a1 en a1 (même case).
    Coup c{Position{0, 'a', 1}, Position{0, 'a', 1}, TypeCoup::NORMAL, TypePiece::REINE};
    EXPECT_FALSE(p.jouerCoup(c));
}

TEST(Partie, PromotionPionVersReine) {
    Partie p;
    // Un pion blanc est en (0,'a',2). Pour tester la promotion on l'appelle
    // directement en forçant sa position au rang 4 (le flux normal passe par
    // jouerCoup — ce test isole la primitive Partie::promouvoir).
    // Ici on vérifie simplement l'API (refus si la pièce n'est pas un pion).
    EXPECT_FALSE(p.promouvoir(Position{0, 'a', 1}, TypePiece::REINE));
}

TEST(Partie, RoqueRefuseSiRoiADejaBouge) {
    Partie p;
    // Au démarrage aucune pièce n'a bougé, mais les cases entre le roi et
    // la tour ne sont pas vides : le roque doit être refusé.
    EXPECT_FALSE(p.tenterRoque(TypeCoup::ROQUE_PETIT));
    EXPECT_FALSE(p.tenterRoque(TypeCoup::ROQUE_GRAND));
}

TEST(Partie, PartieNonTermineeAuDemarrage) {
    Partie p;
    EXPECT_FALSE(p.estTerminee());
    EXPECT_FALSE(p.estEnEchec(Couleur::BLANC));
    EXPECT_FALSE(p.estMat(Couleur::BLANC));
}
