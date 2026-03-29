#!/bin/sh

set -eu

mkdir -p build

xcrun clang++ -std=c++20 \
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
    source/WorldMap.cpp \
    source/character.cpp \
    -o build/rpg-game

printf 'Built %s\n' "build/rpg-game"
