// tests/test_ia.cpp
#include <gtest/gtest.h>
#include "modele/PieceFactory.h"
#include "modele/Partie.h"

using namespace yalta;

TEST(PieceClone, ClonerConserveAttributs) {
    auto piece = PieceFactory::creer(TypePiece::TOUR, Couleur::BLANC,
                                     Position{0, 'e', 1});
    piece->marquerDeplace();
    auto copie = piece->clone();

    EXPECT_EQ(copie->getType(),     TypePiece::TOUR);
    EXPECT_EQ(copie->getCouleur(),  Couleur::BLANC);
    EXPECT_EQ(copie->getPosition(), (Position{0, 'e', 1}));
    EXPECT_TRUE(copie->aDejaBouge());
    EXPECT_NE(copie.get(), piece.get());
}

TEST(PieceClone, ClonePieceNonDeplaceeGardeFlagFalse) {
    auto piece = PieceFactory::creer(TypePiece::PION, Couleur::NOIR,
                                     Position{1, 'a', 2});
    ASSERT_FALSE(piece->aDejaBouge());
    auto copie = piece->clone();
    EXPECT_FALSE(copie->aDejaBouge());
    EXPECT_NE(copie.get(), piece.get());
}

TEST(PartieCopie, CopieIndependante) {
    Partie orig;
    Partie copie = orig;  // copy constructor

    EXPECT_EQ(copie.joueurActif().couleur, orig.joueurActif().couleur);

    auto coups = copie.coupsLegaux();
    ASSERT_FALSE(coups.empty());
    copie.jouerCoup(coups.front());
    EXPECT_EQ(orig.joueurActif().couleur, Couleur::BLANC);
}

TEST(PartieCoupsLegaux, RetourneDesCoups) {
    Partie p;
    auto coups = p.coupsLegaux();
    EXPECT_GT(coups.size(), 0u);
}

TEST(PartieCoupsLegaux, TousCoupsLegaux) {
    Partie p;
    for (const auto& coup : p.coupsLegaux()) {
        Partie copie = p;
        EXPECT_TRUE(copie.jouerCoup(coup))
            << "Coup censé légal a échoué dans jouerCoup";
    }
}
