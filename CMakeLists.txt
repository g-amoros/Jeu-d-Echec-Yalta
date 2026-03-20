cmake_minimum_required(VERSION 3.22)
project(yalta LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Qt 6 — module Widgets pour l'UI.
# find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Squelette : le répertoire src/ sera peuplé progressivement.
# add_subdirectory(src)
# enable_testing()
# add_subdirectory(tests)
