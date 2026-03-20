# Jeu d'Échec Yalta

Implémentation C++ / Qt 6 du jeu d'échecs **Yalta** (variante à 3 joueurs sur plateau de 96 cases), réalisée dans le cadre de l'UCE *Patron de conception avancé*.

## Objectifs

- Moteur de jeu Yalta complet (règles classiques + coups spéciaux : roque, promotion, prise en passant).
- Architecture **MVC** + 2 design patterns supplémentaires (**Strategy**, **Factory**).
- Interface graphique Qt 6 permettant de jouer au clic.
- Détection d'échec, échec et mat, pat, élimination d'un des 3 joueurs.
- *(itération suivante)* IA min-max / alpha-beta multi-threadée simulant 1, 2 ou 3 joueurs.

## Arborescence

```
docs/       diagrammes UML (PlantUML) + rapport LaTeX
src/        code source (modele / vue / controleur)
tests/      tests unitaires (GoogleTest)
```

## Build

Prérequis : CMake ≥ 3.22, Qt 6, compilateur C++20, GoogleTest (fetché par CMake).

```bash
cmake -S . -B build
cmake --build build
./build/yalta
```

## Documentation

- `docs/TECH.md` — choix techniques.
- `docs/diagrammes/` — diagrammes UML.
- `docs/rapport/rapport.pdf` — rapport du projet.
- `doxygen Doxyfile` → documentation générée de l'API.
