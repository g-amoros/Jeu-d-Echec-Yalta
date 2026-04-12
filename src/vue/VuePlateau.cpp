#include "VuePlateau.h"

#include "modele/Partie.h"
#include "modele/Piece.h"

#include <QBrush>
#include <QGraphicsPolygonItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QMouseEvent>
#include <QPen>
#include <QPolygonF>

namespace yalta {

namespace {
constexpr qreal TAILLE_CASE = 60.0;

QString glyphe(TypePiece t, Couleur c) {
    // Caractères Unicode des pièces d'échecs.
    const bool b = (c == Couleur::BLANC);
    switch (t) {
        case TypePiece::ROI:      return b ? "♔" : (c == Couleur::NOIR ? "♚" : "♛");
        case TypePiece::REINE:    return b ? "♕" : "♛";
        case TypePiece::TOUR:     return b ? "♖" : "♜";
        case TypePiece::FOU:      return b ? "♗" : "♝";
        case TypePiece::CAVALIER: return b ? "♘" : "♞";
        case TypePiece::PION:     return b ? "♙" : "♟";
    }
    return "?";
}
} // namespace

VuePlateau::VuePlateau(const Partie& partie, QWidget* parent)
    : QGraphicsView(parent), partie_(partie)
{
    setScene(new QGraphicsScene(this));
    rafraichir();
}

void VuePlateau::rafraichir() {
    scene()->clear();

    // Rendu du plateau : 3 secteurs disposés en triangle.
    for (int s = 0; s < 3; ++s) {
        qreal ox = (s == 0 ? 0 : (s == 1 ? 8 * TAILLE_CASE : 4 * TAILLE_CASE));
        qreal oy = (s == 2 ? -4 * TAILLE_CASE : 0);
        for (int r = 1; r <= 4; ++r) {
            for (char c = 'a'; c <= 'h'; ++c) {
                QRectF rect(ox + (c - 'a') * TAILLE_CASE,
                            oy + (r - 1) * TAILLE_CASE,
                            TAILLE_CASE, TAILLE_CASE);
                auto* item = scene()->addRect(rect);
                bool claire = ((c - 'a') + r + s) % 2 == 0;
                item->setBrush(claire ? Qt::lightGray : Qt::darkGray);
            }
        }
    }

    // Rendu des pièces.
    for (const auto& [pos, piece] : partie_.plateau().cases()) {
        if (!piece) continue;
        qreal ox = (pos.secteur == 0 ? 0 : (pos.secteur == 1 ? 8 * TAILLE_CASE : 4 * TAILLE_CASE));
        qreal oy = (pos.secteur == 2 ? -4 * TAILLE_CASE : 0);
        auto* texte = scene()->addText(glyphe(piece->getType(), piece->getCouleur()));
        texte->setPos(ox + (pos.colonne - 'a') * TAILLE_CASE + 10,
                      oy + (pos.rang - 1)     * TAILLE_CASE + 5);
    }
}

void VuePlateau::surbrillance(const std::vector<Position>& cibles) {
    for (const Position& p : cibles) {
        qreal ox = (p.secteur == 0 ? 0 : (p.secteur == 1 ? 8 * TAILLE_CASE : 4 * TAILLE_CASE));
        qreal oy = (p.secteur == 2 ? -4 * TAILLE_CASE : 0);
        auto* halo = scene()->addRect(
            QRectF(ox + (p.colonne - 'a') * TAILLE_CASE,
                   oy + (p.rang - 1)     * TAILLE_CASE,
                   TAILLE_CASE, TAILLE_CASE));
        halo->setBrush(QBrush(QColor(80, 200, 80, 100)));
    }
}

void VuePlateau::mousePressEvent(QMouseEvent* event) {
    QPointF pt = mapToScene(event->pos());
    int secteur = 0;
    qreal ox = 0, oy = 0;
    if (pt.x() >= 8 * TAILLE_CASE) { secteur = 1; ox = 8 * TAILLE_CASE; }
    else if (pt.y() < 0)           { secteur = 2; ox = 4 * TAILLE_CASE; oy = -4 * TAILLE_CASE; }

    int dc = static_cast<int>((pt.x() - ox) / TAILLE_CASE);
    int dr = static_cast<int>((pt.y() - oy) / TAILLE_CASE);
    if (dc < 0 || dc > 7 || dr < 0 || dr > 3) return;

    emit caseCliquee(Position{static_cast<std::int8_t>(secteur),
                              static_cast<char>('a' + dc),
                              static_cast<std::int8_t>(dr + 1)});
}

} // namespace yalta
