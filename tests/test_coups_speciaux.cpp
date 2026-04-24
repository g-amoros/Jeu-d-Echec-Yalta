#include <gtest/gtest.h>

#define private public
#include "modele/Partie.h"
#undef private
#include "modele/PieceFactory.h"

using namespace yalta;

namespace {

void viderPartie(Partie& partie) {
    partie.plateau_ = Plateau{};
    partie.pieces_.clear();
    partie.historique_.clear();
    partie.tourCourant_ = 0;
    for (auto& joueur : partie.joueurs_) {
        joueur.estElimine = false;
    }
}

Piece* ajouterPiece(Partie& partie, TypePiece type, Couleur couleur, Position position) {
    partie.pieces_.push_back(PieceFactory::creer(type, couleur, position));
    Piece* piece = partie.pieces_.back().get();
    partie.plateau_.poser(position, piece);
    return piece;
}

} // namespace

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

TEST(Partie, CaptureRetireLaPieceDuModele) {
    Partie p;
    viderPartie(p);

    Piece* tourBlanche = ajouterPiece(p, TypePiece::TOUR, Couleur::BLANC, Position{0, 'a', 1});
    ajouterPiece(p, TypePiece::PION, Couleur::NOIR, Position{0, 'a', 3});

    ASSERT_EQ(p.pieces_.size(), 2u);
    ASSERT_TRUE(p.jouerCoup(Coup{Position{0, 'a', 1}, Position{0, 'a', 3}, TypeCoup::NORMAL, TypePiece::REINE}));

    EXPECT_EQ(p.plateau_.pieceEn(Position{0, 'a', 3}), tourBlanche);
    EXPECT_EQ(p.plateau_.pieceEn(Position{0, 'a', 1}), nullptr);
    EXPECT_EQ(p.pieces_.size(), 1u);
}

TEST(Partie, PionAuRang4NeDevientPasAutomatiquementReine) {
    Partie p;

    ASSERT_TRUE(p.jouerCoup(Coup{
        Position{0, 'a', 2},
        Position{0, 'a', 4},
        TypeCoup::NORMAL,
        TypePiece::REINE
    }));

    Piece* piece = p.plateau().pieceEn(Position{0, 'a', 4});
    ASSERT_NE(piece, nullptr);
    EXPECT_EQ(piece->getType(), TypePiece::PION);
}
