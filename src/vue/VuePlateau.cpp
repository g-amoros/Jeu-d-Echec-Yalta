#include "VuePlateau.h"

#include "modele/Partie.h"
#include "modele/Piece.h"

#include <QBrush>
#include <QFont>
#include <QGraphicsEllipseItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QPolygonF>
#include <QtMath>
#include <cmath>

namespace yalta {

namespace {

// ── Géométrie du plateau (inchangée) ──────────────────────────────────────

constexpr qreal UNITE_CASE        = 64.0;
constexpr qreal LONGUEUR_PIECE    = 4.0 * UNITE_CASE;
constexpr qreal LARGEUR_PIECE     = LONGUEUR_PIECE / 1.7320508075688772;
constexpr qreal MARGE_SCENE       = 48.0;

struct TuileQuadrilatere {
    QPointF centre;
    QPointF bordExterieurGauche;
    QPointF bordExterieurDroit;
    QPointF bordInterieurDroit;
};

struct ProjectionCase {
    int pieceIndex;
    int colonneLocale;
};

struct OrientationPiece {
    bool echangerAxes;
    bool inverserColonnes;
    bool inverserRangs;
};

struct CoordonneesLocales {
    int pieceIndex;
    int colonne;
    int rang;
};

qreal rotationPiece(int pieceIndex) {
    return -60.0 + pieceIndex * 60.0;
}

QPointF rotation(QPointF point, qreal angleDegres) {
    const qreal angle = qDegreesToRadians(angleDegres);
    const qreal c = std::cos(angle);
    const qreal s = std::sin(angle);
    return QPointF(point.x() * c - point.y() * s,
                   point.x() * s + point.y() * c);
}

TuileQuadrilatere pieceBase() {
    return {
        QPointF(0.0, 0.0),
        QPointF(0.0, -LONGUEUR_PIECE),
        QPointF(LARGEUR_PIECE, -LONGUEUR_PIECE),
        QPointF(0.8660254037844386 * LONGUEUR_PIECE, -0.5 * LONGUEUR_PIECE)
    };
}

TuileQuadrilatere piecePlateau(int pieceIndex) {
    const TuileQuadrilatere base = pieceBase();
    const qreal angle = rotationPiece(pieceIndex);
    return {
        rotation(base.centre, angle),
        rotation(base.bordExterieurGauche, angle),
        rotation(base.bordExterieurDroit, angle),
        rotation(base.bordInterieurDroit, angle)
    };
}

OrientationPiece orientationPiece(int pieceIndex) {
    return (pieceIndex % 2 == 0)
        ? OrientationPiece{true, false, true}
        : OrientationPiece{false, false, false};
}

ProjectionCase projectionDepuisPosition(Position position) {
    const int colonneAbsolue = position.colonne - 'a';

    switch (position.secteur) {
        case 0:
            if (colonneAbsolue < 4) return {0, colonneAbsolue};
            return {1, colonneAbsolue - 4};
        case 1:
            if (colonneAbsolue < 4) return {2, colonneAbsolue};
            return {3, colonneAbsolue - 4};
        case 2:
            if (colonneAbsolue < 4) return {4, colonneAbsolue};
            return {5, colonneAbsolue - 4};
        default:
            return {0, 0};
    }
}

QPointF interpolationBilineaire(const TuileQuadrilatere& piece, qreal u, qreal v) {
    const QPointF a = piece.centre;
    const QPointF b = piece.bordExterieurGauche;
    const QPointF c = piece.bordExterieurDroit;
    const QPointF d = piece.bordInterieurDroit;

    return a * ((1.0 - u) * (1.0 - v))
         + b * ((1.0 - u) * v)
         + c * (u * v)
         + d * (u * (1.0 - v));
}

CoordonneesLocales coordonneesLocales(Position position) {
    const ProjectionCase projection = projectionDepuisPosition(position);
    const OrientationPiece orientation = orientationPiece(projection.pieceIndex);

    int colonneLocale = projection.colonneLocale;
    int rangLocal = 4 - position.rang;

    if (orientation.echangerAxes) std::swap(colonneLocale, rangLocal);
    if (orientation.inverserColonnes) colonneLocale = 3 - colonneLocale;
    if (orientation.inverserRangs) rangLocal = 3 - rangLocal;

    return {projection.pieceIndex, colonneLocale, rangLocal};
}

QPolygonF polygoneCase(Position position) {
    const CoordonneesLocales coords = coordonneesLocales(position);
    const TuileQuadrilatere piece = piecePlateau(coords.pieceIndex);

    const qreal u0 = coords.colonne / 4.0;
    const qreal u1 = (coords.colonne + 1) / 4.0;
    const qreal v0 = coords.rang / 4.0;
    const qreal v1 = (coords.rang + 1) / 4.0;

    QPolygonF poly;
    poly << interpolationBilineaire(piece, u0, v0)
         << interpolationBilineaire(piece, u1, v0)
         << interpolationBilineaire(piece, u1, v1)
         << interpolationBilineaire(piece, u0, v1);
    return poly;
}

QPointF centreCase(Position position) {
    const QPolygonF poly = polygoneCase(position);
    QPointF somme;
    for (const QPointF& point : poly) somme += point;
    return somme / poly.size();
}

QColor couleurCase(Position position) {
    const CoordonneesLocales coords = coordonneesLocales(position);
    return ((coords.pieceIndex + coords.colonne + coords.rang) % 2 == 0)
        ? QColor(235, 210, 170)
        : QColor(160, 110, 70);
}

QString glyphe(TypePiece t) {
    switch (t) {
        case TypePiece::ROI:      return QStringLiteral("♚");
        case TypePiece::REINE:    return QStringLiteral("♛");
        case TypePiece::TOUR:     return QStringLiteral("♜");
        case TypePiece::FOU:      return QStringLiteral("♝");
        case TypePiece::CAVALIER: return QStringLiteral("♞");
        case TypePiece::PION:     return QStringLiteral("♟");
    }
    return QStringLiteral("?");
}

// ── Visuels des pièces ────────────────────────────────────────────────────

struct StylePiece {
    QColor cercle;   // fond du token
    QColor bord;     // contour du token
    QColor glyphe;   // couleur du symbole unicode
};

StylePiece stylePiece(Couleur c) {
    switch (c) {
        case Couleur::BLANC:
            // Token ivoire clair, symbole presque noir
            return {QColor(242, 228, 192, 230),
                    QColor(195, 162, 100, 255),
                    QColor(28, 22, 12)};
        case Couleur::NOIR:
            // Token brun foncé, symbole crème
            return {QColor(46, 36, 24, 230),
                    QColor(95, 78, 52, 255),
                    QColor(236, 222, 192)};
        case Couleur::ROUGE:
            // Token rouge profond, symbole rose clair
            return {QColor(168, 18, 18, 230),
                    QColor(225, 60, 60, 255),
                    QColor(255, 210, 200)};
    }
    return {Qt::gray, Qt::black, Qt::white};
}

} // namespace

// ── Construction ─────────────────────────────────────────────────────────

VuePlateau::VuePlateau(const Partie& partie, QWidget* parent)
    : QGraphicsView(parent), partie_(partie)
{
    setScene(new QGraphicsScene(this));
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setBackgroundBrush(QColor(28, 28, 34));
    setFrameShape(QFrame::NoFrame);
    rafraichir();
}

// ── Rendu complet ─────────────────────────────────────────────────────────

void VuePlateau::rafraichir() {
    scene()->clear();

    // 1. Plateau (géométrie strictement inchangée)
    for (int secteur = 0; secteur < 3; ++secteur) {
        for (int rang = 1; rang <= 4; ++rang) {
            for (char colonne = 'a'; colonne <= 'h'; ++colonne) {
                const Position pos{
                    static_cast<std::int8_t>(secteur),
                    colonne,
                    static_cast<std::int8_t>(rang)
                };
                scene()->addPolygon(
                    polygoneCase(pos),
                    QPen(QColor(30, 30, 30), 1.1),
                    QBrush(couleurCase(pos))
                );
            }
        }
    }

    // 2. Dernier coup (halo bleu-ardoise)
    if (dernierCoup_) {
        const QPen  bordBleu(QColor(85, 145, 230, 200), 2.5);
        const QBrush fondBleu(QColor(75, 130, 210, 95));
        scene()->addPolygon(polygoneCase(dernierCoup_->origine),     bordBleu, fondBleu);
        scene()->addPolygon(polygoneCase(dernierCoup_->destination),  bordBleu, fondBleu);
    }

    // 3. Pièce sélectionnée (halo doré)
    if (selection_) {
        scene()->addPolygon(
            polygoneCase(*selection_),
            QPen(QColor(242, 192, 28, 230), 3.2),
            QBrush(QColor(242, 192, 28, 75))
        );
    }

    // 4. Destinations légales — point vert (case vide) ou anneau (capture)
    for (const Position& pos : surbrillance_) {
        const bool occupe = partie_.plateau().pieceEn(pos) != nullptr;
        if (occupe) {
            scene()->addPolygon(
                polygoneCase(pos),
                QPen(QColor(55, 190, 90, 215), 3.5),
                QBrush(QColor(55, 190, 90, 48))
            );
        } else {
            constexpr qreal R = 9.5;
            const QPointF c = centreCase(pos);
            scene()->addEllipse(
                c.x() - R, c.y() - R, R * 2.0, R * 2.0,
                QPen(Qt::NoPen),
                QBrush(QColor(55, 190, 90, 185))
            );
        }
    }

    // 5. Pièces — token circulaire + glyphe unicode
    QFont police;
    police.setPointSize(38);
    police.setStyleStrategy(QFont::PreferAntialias);

    for (const auto& [pos, piece] : partie_.plateau().cases()) {
        if (!piece) continue;

        const QPointF   c     = centreCase(pos);
        const StylePiece style = stylePiece(piece->getCouleur());

        // Token (cercle de fond)
        constexpr qreal R = 26.0;
        scene()->addEllipse(
            c.x() - R, c.y() - R, R * 2.0, R * 2.0,
            QPen(style.bord, 2.2),
            QBrush(style.cercle)
        );

        // Glyphe centré
        auto* texte = scene()->addText(glyphe(piece->getType()), police);
        texte->setDefaultTextColor(style.glyphe);
        const QRectF br = texte->boundingRect();
        texte->setPos(c.x() - br.width()  / 2.0,
                      c.y() - br.height() / 2.0);
    }

    setSceneRect(scene()->itemsBoundingRect().adjusted(
        -MARGE_SCENE, -MARGE_SCENE, MARGE_SCENE, MARGE_SCENE));
}

// ── Mutateurs d'état (stockent sans redessiner) ───────────────────────────

void VuePlateau::surbrillance(const std::vector<Position>& cibles) {
    surbrillance_ = cibles;
}

void VuePlateau::selectionner(std::optional<Position> pos) {
    selection_ = pos;
}

void VuePlateau::setDernierCoup(std::optional<Coup> coup) {
    dernierCoup_ = coup;
}

// ── Entrée souris ─────────────────────────────────────────────────────────

void VuePlateau::mousePressEvent(QMouseEvent* event) {
    const QPointF pt = mapToScene(event->pos());

    for (int secteur = 0; secteur < 3; ++secteur) {
        for (int rang = 1; rang <= 4; ++rang) {
            for (char colonne = 'a'; colonne <= 'h'; ++colonne) {
                const Position pos{
                    static_cast<std::int8_t>(secteur),
                    colonne,
                    static_cast<std::int8_t>(rang)
                };
                if (!polygoneCase(pos).containsPoint(pt, Qt::OddEvenFill)) continue;
                emit caseCliquee(pos);
                return;
            }
        }
    }
}

} // namespace yalta
