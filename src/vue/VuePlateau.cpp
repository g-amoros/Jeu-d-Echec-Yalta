#include "VuePlateau.h"

#include "modele/Partie.h"
#include "modele/Piece.h"

#include <QBrush>
#include <QFont>
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

QColor couleurPiece(Couleur c) {
    switch (c) {
        case Couleur::BLANC: return QColor(245, 245, 245);
        case Couleur::NOIR:  return QColor(25, 25, 25);
        case Couleur::ROUGE: return QColor(200, 40, 40);
    }
    return Qt::gray;
}

QColor couleurCase(Position position) {
    const CoordonneesLocales coords = coordonneesLocales(position);
    return ((coords.pieceIndex + coords.colonne + coords.rang) % 2 == 0)
        ? QColor(235, 210, 170)
        : QColor(160, 110, 70);
}

} // namespace

VuePlateau::VuePlateau(const Partie& partie, QWidget* parent)
    : QGraphicsView(parent), partie_(partie)
{
    setScene(new QGraphicsScene(this));
    setRenderHint(QPainter::Antialiasing);
    setBackgroundBrush(QColor(40, 40, 45));
    rafraichir();
}

void VuePlateau::rafraichir() {
    scene()->clear();

    for (int secteur = 0; secteur < 3; ++secteur) {
        for (int rang = 1; rang <= 4; ++rang) {
            for (char colonne = 'a'; colonne <= 'h'; ++colonne) {
                const Position position{
                    static_cast<std::int8_t>(secteur),
                    colonne,
                    static_cast<std::int8_t>(rang)
                };

                scene()->addPolygon(
                    polygoneCase(position),
                    QPen(QColor(30, 30, 30), 1.1),
                    QBrush(couleurCase(position))
                );
            }
        }
    }

    QFont police;
    police.setPointSize(48);

    for (const auto& [pos, piece] : partie_.plateau().cases()) {
        if (!piece) continue;

        const QPointF centre = centreCase(pos);
        auto* texte = scene()->addText(glyphe(piece->getType()), police);
        texte->setDefaultTextColor(couleurPiece(piece->getCouleur()));

        const QRectF br = texte->boundingRect();
        texte->setPos(centre.x() - br.width() / 2.0,
                      centre.y() - br.height() / 2.0);
    }

    setSceneRect(scene()->itemsBoundingRect().adjusted(-MARGE_SCENE, -MARGE_SCENE,
                                                       MARGE_SCENE, MARGE_SCENE));
}

void VuePlateau::surbrillance(const std::vector<Position>& cibles) {
    for (const Position& position : cibles) {
        scene()->addPolygon(
            polygoneCase(position),
            QPen(QColor(40, 140, 40), 3),
            QBrush(QColor(80, 200, 80, 140))
        );
    }
}

void VuePlateau::mousePressEvent(QMouseEvent* event) {
    const QPointF pt = mapToScene(event->pos());

    for (int secteur = 0; secteur < 3; ++secteur) {
        for (int rang = 1; rang <= 4; ++rang) {
            for (char colonne = 'a'; colonne <= 'h'; ++colonne) {
                const Position position{
                    static_cast<std::int8_t>(secteur),
                    colonne,
                    static_cast<std::int8_t>(rang)
                };

                if (!polygoneCase(position).containsPoint(pt, Qt::OddEvenFill)) continue;
                emit caseCliquee(position);
                return;
            }
        }
    }
}

} // namespace yalta
