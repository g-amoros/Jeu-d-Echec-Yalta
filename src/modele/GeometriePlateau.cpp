#include "GeometriePlateau.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <unordered_map>

namespace yalta::geometrie {

namespace {

constexpr double UNITE_CASE = 64.0;
constexpr double LONGUEUR_PIECE = 4.0 * UNITE_CASE;
constexpr double LARGEUR_PIECE = LONGUEUR_PIECE / 1.7320508075688772;
constexpr double SEUIL_RAYON_ARETE = 0.7;
constexpr double SEUIL_RAYON_COIN = 0.6;
constexpr double SEUIL_CAVALIER_PERP = 0.35;

struct Point {
    double x {0.0};
    double y {0.0};
};

struct RoundedPoint {
    long long x {0};
    long long y {0};

    friend bool operator==(const RoundedPoint& lhs, const RoundedPoint& rhs) noexcept {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
};

struct RoundedPointHash {
    std::size_t operator()(const RoundedPoint& point) const noexcept {
        return static_cast<std::size_t>(point.x * 1315423911ULL ^ point.y);
    }
};

struct TuileQuadrilatere {
    Point centre;
    Point bordExterieurGauche;
    Point bordExterieurDroit;
    Point bordInterieurDroit;
};

struct ProjectionCase {
    int pieceIndex {0};
    int colonneLocale {0};
};

struct OrientationPiece {
    bool echangerAxes {false};
    bool inverserColonnes {false};
    bool inverserRangs {false};
};

struct CoordonneesLocales {
    int pieceIndex {0};
    int colonne {0};
    int rang {0};
};

struct TablesGeometrie {
    std::array<Position, 96> positions {};
    std::array<Point, 96> centres {};
    std::array<std::vector<Position>, 96> voisinsArete {};
    std::array<std::vector<Position>, 96> voisinsCoin {};
    std::array<std::vector<std::vector<Position>>, 96> rayonsArete {};
    std::array<std::vector<std::vector<Position>>, 96> rayonsCoin {};
    std::array<std::vector<Position>, 96> sautsCavalier {};
};

Point rotation(Point point, double angleDegres) {
    const double angle = angleDegres * M_PI / 180.0;
    const double c = std::cos(angle);
    const double s = std::sin(angle);
    return {point.x * c - point.y * s, point.x * s + point.y * c};
}

double rotationPiece(int pieceIndex) {
    return -60.0 + pieceIndex * 60.0;
}

TuileQuadrilatere pieceBase() {
    return {
        {0.0, 0.0},
        {0.0, -LONGUEUR_PIECE},
        {LARGEUR_PIECE, -LONGUEUR_PIECE},
        {0.8660254037844386 * LONGUEUR_PIECE, -0.5 * LONGUEUR_PIECE}
    };
}

TuileQuadrilatere piecePlateau(int pieceIndex) {
    const TuileQuadrilatere base = pieceBase();
    const double angle = rotationPiece(pieceIndex);
    return {
        rotation(base.centre, angle),
        rotation(base.bordExterieurGauche, angle),
        rotation(base.bordExterieurDroit, angle),
        rotation(base.bordInterieurDroit, angle)
    };
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

OrientationPiece orientationPiece(int pieceIndex) {
    if (pieceIndex % 2 == 0) {
        return {true, false, true};
    }
    return {false, false, false};
}

CoordonneesLocales coordonneesLocales(Position position) {
    const ProjectionCase projection = projectionDepuisPosition(position);
    const OrientationPiece orientation = orientationPiece(projection.pieceIndex);

    int colonneLocale = projection.colonneLocale;
    int rangLocal = 4 - position.rang;

    if (orientation.echangerAxes) {
        std::swap(colonneLocale, rangLocal);
    }
    if (orientation.inverserColonnes) {
        colonneLocale = 3 - colonneLocale;
    }
    if (orientation.inverserRangs) {
        rangLocal = 3 - rangLocal;
    }

    return {projection.pieceIndex, colonneLocale, rangLocal};
}

Point interpolationBilineaire(const TuileQuadrilatere& piece, double u, double v) {
    const Point& a = piece.centre;
    const Point& b = piece.bordExterieurGauche;
    const Point& c = piece.bordExterieurDroit;
    const Point& d = piece.bordInterieurDroit;

    return {
        a.x * ((1.0 - u) * (1.0 - v)) + b.x * ((1.0 - u) * v) + c.x * (u * v) + d.x * (u * (1.0 - v)),
        a.y * ((1.0 - u) * (1.0 - v)) + b.y * ((1.0 - u) * v) + c.y * (u * v) + d.y * (u * (1.0 - v))
    };
}

std::array<Point, 4> polygoneCase(Position position) {
    const CoordonneesLocales coords = coordonneesLocales(position);
    const TuileQuadrilatere piece = piecePlateau(coords.pieceIndex);

    const double u0 = coords.colonne / 4.0;
    const double u1 = (coords.colonne + 1) / 4.0;
    const double v0 = coords.rang / 4.0;
    const double v1 = (coords.rang + 1) / 4.0;

    return {
        interpolationBilineaire(piece, u0, v0),
        interpolationBilineaire(piece, u1, v0),
        interpolationBilineaire(piece, u1, v1),
        interpolationBilineaire(piece, u0, v1)
    };
}

Point centreCase(Position position) {
    const auto polygone = polygoneCase(position);
    Point centre {};
    for (const Point& point : polygone) {
        centre.x += point.x;
        centre.y += point.y;
    }
    centre.x /= polygone.size();
    centre.y /= polygone.size();
    return centre;
}

RoundedPoint arrondir(Point point) {
    constexpr double facteur = 1'000'000.0;
    return {
        static_cast<long long>(std::llround(point.x * facteur)),
        static_cast<long long>(std::llround(point.y * facteur))
    };
}

int indexPosition(Position position) {
    return static_cast<int>(position.secteur) * 32
         + (static_cast<int>(position.rang) - 1) * 8
         + (position.colonne - 'a');
}

Point vecteur(Point origine, Point destination) {
    return {destination.x - origine.x, destination.y - origine.y};
}

double norme(Point vecteur) {
    return std::hypot(vecteur.x, vecteur.y);
}

double cosinus(Point lhs, Point rhs) {
    const double denom = norme(lhs) * norme(rhs);
    if (denom == 0.0) {
        return -1.0;
    }
    return (lhs.x * rhs.x + lhs.y * rhs.y) / denom;
}

std::vector<Position> dedupliquer(std::vector<Position> positions) {
    std::sort(positions.begin(), positions.end(), [](const Position& lhs, const Position& rhs) {
        if (lhs.secteur != rhs.secteur) return lhs.secteur < rhs.secteur;
        if (lhs.rang != rhs.rang) return lhs.rang < rhs.rang;
        return lhs.colonne < rhs.colonne;
    });
    positions.erase(std::unique(positions.begin(), positions.end()), positions.end());
    return positions;
}

std::vector<std::vector<Position>> construireRayons(
    const std::array<Point, 96>& centres,
    const std::array<std::vector<Position>, 96>& voisins,
    Position origine,
    double seuilCosinus)
{
    std::vector<std::vector<Position>> rayons;

    for (const Position& premier : voisins[indexPosition(origine)]) {
        std::vector<Position> rayon {premier};
        Position precedent = origine;
        Position courant = premier;

        while (true) {
            const Point directionPrecedente = vecteur(
                centres[indexPosition(precedent)],
                centres[indexPosition(courant)]);

            double meilleurScore = -2.0;
            Position meilleur {};
            bool trouve = false;

            for (const Position& candidat : voisins[indexPosition(courant)]) {
                if (candidat == precedent) {
                    continue;
                }
                if (std::find(rayon.begin(), rayon.end(), candidat) != rayon.end()) {
                    continue;
                }

                const double score = cosinus(
                    directionPrecedente,
                    vecteur(centres[indexPosition(courant)], centres[indexPosition(candidat)]));
                if (score > meilleurScore) {
                    meilleurScore = score;
                    meilleur = candidat;
                    trouve = true;
                }
            }

            if (!trouve || meilleurScore < seuilCosinus) {
                break;
            }

            rayon.push_back(meilleur);
            precedent = courant;
            courant = meilleur;
        }

        rayons.push_back(std::move(rayon));
    }

    return rayons;
}

std::vector<Position> construireSautsCavalier(
    const std::array<Point, 96>& centres,
    const std::array<std::vector<Position>, 96>& voisinsArete,
    const std::array<std::vector<std::vector<Position>>, 96>& rayonsArete,
    Position origine)
{
    std::vector<Position> sauts;

    for (const auto& rayon : rayonsArete[indexPosition(origine)]) {
        if (rayon.size() < 2) {
            continue;
        }

        const Position premier = rayon[0];
        const Position second = rayon[1];
        const Point direction = vecteur(centres[indexPosition(premier)], centres[indexPosition(second)]);

        for (const Position& candidat : voisinsArete[indexPosition(second)]) {
            if (candidat == premier) {
                continue;
            }

            const double perpendicularite = std::abs(cosinus(
                direction,
                vecteur(centres[indexPosition(second)], centres[indexPosition(candidat)])));
            if (perpendicularite < SEUIL_CAVALIER_PERP) {
                sauts.push_back(candidat);
            }
        }
    }

    return dedupliquer(std::move(sauts));
}

TablesGeometrie construireTables() {
    TablesGeometrie tables;

    for (int secteur = 0; secteur < 3; ++secteur) {
        for (int rang = 1; rang <= 4; ++rang) {
            for (char colonne = 'a'; colonne <= 'h'; ++colonne) {
                const Position position {
                    static_cast<std::int8_t>(secteur),
                    colonne,
                    static_cast<std::int8_t>(rang)
                };
                const int index = indexPosition(position);
                tables.positions[index] = position;
                tables.centres[index] = centreCase(position);
            }
        }
    }

    std::array<std::unordered_map<RoundedPoint, int, RoundedPointHash>, 96> sommets;
    for (const Position& position : tables.positions) {
        std::unordered_map<RoundedPoint, int, RoundedPointHash> compte;
        for (const Point& sommet : polygoneCase(position)) {
            ++compte[arrondir(sommet)];
        }
        sommets[indexPosition(position)] = std::move(compte);
    }

    for (std::size_t i = 0; i < tables.positions.size(); ++i) {
        for (std::size_t j = i + 1; j < tables.positions.size(); ++j) {
            int sommetsCommuns = 0;
            for (const auto& [sommet, _] : sommets[i]) {
                if (sommets[j].contains(sommet)) {
                    ++sommetsCommuns;
                }
            }

            if (sommetsCommuns >= 2) {
                tables.voisinsArete[i].push_back(tables.positions[j]);
                tables.voisinsArete[j].push_back(tables.positions[i]);
            } else if (sommetsCommuns == 1) {
                tables.voisinsCoin[i].push_back(tables.positions[j]);
                tables.voisinsCoin[j].push_back(tables.positions[i]);
            }
        }
    }

    for (const Position& position : tables.positions) {
        const int index = indexPosition(position);
        tables.rayonsArete[index] = construireRayons(
            tables.centres, tables.voisinsArete, position, SEUIL_RAYON_ARETE);
        tables.rayonsCoin[index] = construireRayons(
            tables.centres, tables.voisinsCoin, position, SEUIL_RAYON_COIN);
        tables.sautsCavalier[index] = construireSautsCavalier(
            tables.centres, tables.voisinsArete, tables.rayonsArete, position);
    }

    return tables;
}

const TablesGeometrie& tables() {
    static const TablesGeometrie instance = construireTables();
    return instance;
}

} // namespace

const std::vector<Position>& voisinsArete(Position position) {
    return tables().voisinsArete[indexPosition(position)];
}

const std::vector<Position>& voisinsCoin(Position position) {
    return tables().voisinsCoin[indexPosition(position)];
}

std::vector<std::vector<Position>> rayonsArete(Position position) {
    return tables().rayonsArete[indexPosition(position)];
}

std::vector<std::vector<Position>> rayonsCoin(Position position) {
    return tables().rayonsCoin[indexPosition(position)];
}

std::vector<Position> sautsCavalier(Position position) {
    return tables().sautsCavalier[indexPosition(position)];
}

} // namespace yalta::geometrie
