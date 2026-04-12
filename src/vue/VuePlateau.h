#pragma once

#include "modele/Position.h"
#include <QGraphicsView>

namespace yalta {

class Partie;

/**
 * @brief Rendu graphique du plateau Yalta.
 *
 * Chaque case est un QGraphicsPolygonItem (quadrilatère déformé pour le
 * plateau hexagonal), chaque pièce un QGraphicsPixmapItem. Le clic sur une
 * case émet caseCliquee(Position).
 */
class VuePlateau : public QGraphicsView {
    Q_OBJECT

public:
    explicit VuePlateau(const Partie& partie, QWidget* parent = nullptr);

    /// Redessine le plateau depuis le modèle.
    void rafraichir();

    /// Met en surbrillance les cases de @p cibles (coups légaux affichés).
    void surbrillance(const std::vector<Position>& cibles);

signals:
    void caseCliquee(Position pos);
    void promotionDemandee(Position pos);

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    const Partie& partie_;
};

} // namespace yalta
