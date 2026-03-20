# Choix techniques

Ce document récapitule les briques retenues pour le projet Yalta et la justification de chaque choix.

## Langage : C++20

- Standard imposé implicitement par l'UCE (programmation objet avancée).
- Apports concrets exploités : `std::unique_ptr` / `std::shared_ptr` pour la gestion des pièces, `std::optional` pour représenter « pas de pièce sur la case », `std::variant` pour distinguer les types de coups (normal / roque / promotion / prise en passant), `concepts` pour typer l'interface des Strategy.

## Bibliothèque graphique : Qt 6 (Widgets + QGraphicsScene)

- **QGraphicsScene** est parfaitement adapté au plateau hexagonal Yalta (96 cases non alignées sur une grille cartésienne) : chaque case est un `QGraphicsPolygonItem` positionné librement.
- Le système **signals/slots** de Qt fournit nativement un mécanisme d'Observer, utilisé pour notifier la Vue des modifications du Modèle (ex : `coupJoue(Position, Position)`, `echecDetecte(Joueur)`).
- Qt 6 est multi-plateforme (Linux / macOS / Windows), documenté, et dispose d'un générateur d'images (`QPainter`) suffisant pour un rendu soigné sans passer par OpenGL.

## Build : CMake ≥ 3.22

- Standard de fait en C++ moderne, intégration native de Qt 6 via `find_package(Qt6 COMPONENTS Widgets)` et `CMAKE_AUTOMOC`.
- Permet d'inclure GoogleTest via `FetchContent` sans dépendance système.

## Tests : GoogleTest

- Tests unitaires des règles de déplacement par pièce, détection d'échec/mat/pat, coups spéciaux.
- Lancement via `ctest` intégré à CMake.

## Documentation : Doxygen

- Explicitement mentionné dans le sujet.
- Commentaires `/** */` sur chaque classe publique et chaque méthode non triviale.

## Diagrammes : PlantUML

- Diagrammes versionnés en texte (`.puml`), diffables dans git, régénérables sans outil graphique.
- Trois diagrammes retenus : classes, états-transitions, cas d'utilisation.

## Rapport : LaTeX (article, 11pt)

- Contrainte du sujet : 3 pages maximum.
- Compilation via `latexmk -pdf`.

## Design patterns (3)

1. **Modèle-Vue-Contrôleur** (obligatoire) — sépare l'état du jeu (Modèle), son rendu (Vue Qt) et la traduction des entrées utilisateur (Contrôleur).
2. **Strategy** — interface `IStrategieDeplacement` implémentée par pièce. Permettra d'ajouter la Strategy IA (min-max / alpha-beta) sans toucher au moteur.
3. **Factory** — `PieceFactory` centralise la création des pièces et la disposition initiale des 3 camps Yalta.
