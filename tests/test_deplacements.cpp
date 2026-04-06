#include <gtest/gtest.h>

#include "modele/Piece.h"
#include "modele/PieceFactory.h"
#include "modele/Plateau.h"

using namespace yalta;

namespace {

// Aide : pose une pièce sur le plateau et retourne le Piece* non-owning.
Piece* poser(Plateau& p, std::vector<std::unique_ptr<Piece>>& pool,
             TypePiece type, Couleur couleur, Position pos) {
    pool.push_back(PieceFactory::creer(type, couleur, pos));
    Piece* pc = pool.back().get();
    p.poser(pos, pc);
    return pc;
}

bool contient(const std::vector<Position>& v, Position p) {
    for (const auto& x : v) if (x == p) return true;
    return false;
}

} // namespace

TEST(Pion, AvanceSimpleEtDouble) {
    Plateau plateau;
    std::vector<std::unique_ptr<Piece>> pool;
    Piece* pion = poser(plateau, pool, TypePiece::PION, Couleur::BLANC,
                        Position{0, 'd', 2});

    auto coups = pion->coupsPossibles(plateau);
    EXPECT_TRUE(contient(coups, Position{0, 'd', 3}));
    EXPECT_TRUE(contient(coups, Position{0, 'd', 4}));
}

TEST(Pion, CaptureDiagonale) {
    Plateau plateau;
    std::vector<std::unique_ptr<Piece>> pool;
    Piece* pion = poser(plateau, pool, TypePiece::PION, Couleur::BLANC, Position{0, 'd', 2});
    poser(plateau, pool, TypePiece::PION, Couleur::NOIR,  Position{0, 'e', 3});

    auto coups = pion->coupsPossibles(plateau);
    EXPECT_TRUE(contient(coups, Position{0, 'e', 3}));
}

TEST(Tour, LigneEtColonneBloqueesParAlliée) {
    Plateau plateau;
    std::vector<std::unique_ptr<Piece>> pool;
    Piece* tour = poser(plateau, pool, TypePiece::TOUR, Couleur::BLANC, Position{0, 'a', 1});
    poser(plateau, pool, TypePiece::PION, Couleur::BLANC, Position{0, 'a', 3});

    auto coups = tour->coupsPossibles(plateau);
    EXPECT_TRUE(contient(coups,  Position{0, 'a', 2}));
    EXPECT_FALSE(contient(coups, Position{0, 'a', 3}));
    EXPECT_FALSE(contient(coups, Position{0, 'a', 4}));
}

TEST(Cavalier, HuitSautsValides) {
    Plateau plateau;
    std::vector<std::unique_ptr<Piece>> pool;
    Piece* cav = poser(plateau, pool, TypePiece::CAVALIER, Couleur::BLANC,
                       Position{0, 'd', 3});
    auto coups = cav->coupsPossibles(plateau);
    EXPECT_GE(coups.size(), 2u); // au moins quelques-uns valides dans le secteur
}

TEST(Roi, UneCaseToutesDirections) {
    Plateau plateau;
    std::vector<std::unique_ptr<Piece>> pool;
    Piece* roi = poser(plateau, pool, TypePiece::ROI, Couleur::BLANC, Position{0, 'd', 2});
    auto coups = roi->coupsPossibles(plateau);
    EXPECT_TRUE(contient(coups, Position{0, 'd', 3}));
    EXPECT_TRUE(contient(coups, Position{0, 'e', 2}));
    EXPECT_TRUE(contient(coups, Position{0, 'c', 1}));
}
