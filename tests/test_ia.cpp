// tests/test_ia.cpp
#include <gtest/gtest.h>
#include "ia/IAMinMax.h"
#include "modele/PieceFactory.h"
#include "modele/Partie.h"

#include <atomic>
#include <thread>

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

TEST(IAMinMax, RetourneUnCoupLegal) {
    Partie p;
    IAMinMax ia(2);
    Coup coup = ia.meilleurCoup(p);

    Partie copie = p;
    EXPECT_TRUE(copie.jouerCoup(coup));
}

TEST(IAMinMax, CoupDifferentDePosition0) {
    Partie p;
    IAMinMax ia(2);
    Coup coup = ia.meilleurCoup(p);

    EXPECT_NE(coup.origine, (Position{}));
}

TEST(IAMinMax, MultiThreadSansCrash) {
    Partie p;
    IAMinMax ia(2);
    std::atomic<int> ok{0};

    auto task = [&]() {
        Coup c = ia.meilleurCoup(p);
        if (c.origine != Position{}) {
            ++ok;
        }
    };

    std::thread t1(task);
    std::thread t2(task);
    t1.join();
    t2.join();

    EXPECT_EQ(ok.load(), 2);
}

TEST(IAMinMax, TroisIAPeuventEnchainerPlusieursCoups) {
    Partie p;

    for (int demiCoups = 0; demiCoups < 10 && !p.estTerminee(); ++demiCoups) {
        IAMinMax ia(1);
        const Coup coup = ia.meilleurCoup(p);
        EXPECT_TRUE(p.jouerCoup(coup)) << "Coup IA illégal au demi-coup " << demiCoups;
    }
}

TEST(IAMinMax, PremierCoupDePionNeLePromeutPasEnReine) {
    Partie p;
    IAMinMax ia(1);

    const Coup coup = ia.meilleurCoup(p);
    Piece* avant = p.plateau().pieceEn(coup.origine);
    ASSERT_NE(avant, nullptr);
    ASSERT_EQ(avant->getType(), TypePiece::PION);

    ASSERT_TRUE(p.jouerCoup(coup));
    Piece* apres = p.plateau().pieceEn(coup.destination);
    ASSERT_NE(apres, nullptr);
    EXPECT_EQ(apres->getType(), TypePiece::PION);
}
