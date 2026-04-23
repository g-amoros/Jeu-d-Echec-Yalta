cmake_minimum_required(VERSION 3.22)
project(yalta LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Bibliothèque modèle (sans Qt pour rester testable).
add_library(yalta_modele STATIC
    src/modele/Piece.cpp
    src/modele/Plateau.cpp
    src/modele/PieceFactory.cpp
    src/modele/Partie.cpp
    src/modele/strategies/DeplacementPion.cpp
    src/modele/strategies/DeplacementTour.cpp
    src/modele/strategies/DeplacementCavalier.cpp
    src/modele/strategies/DeplacementFou.cpp
    src/modele/strategies/DeplacementReine.cpp
    src/modele/strategies/DeplacementRoi.cpp
)
target_include_directories(yalta_modele PUBLIC src)

# Exécutable Qt.
add_executable(yalta
    src/main.cpp
    src/vue/FenetrePrincipale.cpp
    src/vue/VuePlateau.cpp
    src/controleur/ControleurJeu.cpp
)
target_include_directories(yalta PRIVATE src)
target_link_libraries(yalta PRIVATE yalta_modele Qt6::Widgets)

# Tests GoogleTest (fetch automatique).
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(yalta_tests
    tests/test_deplacements.cpp
    tests/test_coups_speciaux.cpp
    tests/test_ia.cpp
)
target_link_libraries(yalta_tests PRIVATE yalta_modele GTest::gtest_main)
target_include_directories(yalta_tests PRIVATE src)
include(GoogleTest)
gtest_discover_tests(yalta_tests)
