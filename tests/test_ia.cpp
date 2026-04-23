// tests/test_ia.cpp
#include <gtest/gtest.h>
#include "modele/Partie.h"
#include "modele/PieceFactory.h"

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
    EXPECT_NE(copie.get(), piece.get());  // objet distinct
}
