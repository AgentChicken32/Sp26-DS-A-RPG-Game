#!/bin/sh

set -eu

mkdir -p build

framework_flags=""
compiler="${CXX:-c++}"

if [ "$(uname -s)" = "Darwin" ]; then
    compiler="xcrun clang++"
    framework_flags="-framework AudioToolbox -framework CoreFoundation"
fi

$compiler -std=c++20 \
    source/ActionObject.cpp \
    source/BattleClass.cpp \
    source/DialogueDatabase.cpp \
    source/GameItems.cpp \
    source/Inventory.cpp \
    source/LootTable.cpp \
    source/Main.cpp \
    source/RPG_T_Menu.cpp \
    source/SaveState.cpp \
    source/SceneClass.cpp \
    source/Sound.cpp \
    source/WorldMap.cpp \
    source/character.cpp \
    $framework_flags \
    -o build/rpg-game

printf 'Built %s\n' "build/rpg-game"
