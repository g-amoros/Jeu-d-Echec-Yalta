#pragma once

#include "modele/Coup.h"
#include "modele/Position.h"
#include <QGraphicsView>
#include <optional>
#include <vector>

class QMouseEvent;

namespace yalta {

class Partie;

class VuePlateau : public QGraphicsView {
    Q_OBJECT

public:
    explicit VuePlateau(const Partie& partie, QWidget* parent = nullptr);

    /// Redessine complètement la scène depuis le modèle + état stocké.
    void rafraichir();

    /// Stocke les destinations légales (points verts) ; appeler rafraichir() ensuite.
    void surbrillance(const std::vector<Position>& cibles);

    /// Stocke la case sélectionnée (halo doré) ; appeler rafraichir() ensuite.
    void selectionner(std::optional<Position> pos);

    /// Stocke le dernier coup joué (halo bleu) ; appeler rafraichir() ensuite.
    void setDernierCoup(std::optional<Coup> coup);

signals:
    void caseCliquee(Position pos);

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    const Partie&            partie_;
    std::vector<Position>    surbrillance_;
    std::optional<Position>  selection_;
    std::optional<Coup>      dernierCoup_;
};

} // namespace yalta
