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
    -Isource \
    source/data/ActionObject.cpp \
    source/world/AdventureFlow.cpp \
    source/battle/BattleClass.cpp \
    source/world/DialogueTree.cpp \
    source/world/Gambling.cpp \
    source/inventory/GameItems.cpp \
    source/inventory/Inventory.cpp \
    source/inventory/InventoryMenu.cpp \
    source/battle/LootTable.cpp \
    source/app/Main.cpp \
    source/persistence/SaveState.cpp \
    source/battle/SceneClass.cpp \
    source/platform/Sound.cpp \
    source/platform/UiCommon.cpp \
    source/world/WorldUi.cpp \
    source/world/WorldMap.cpp \
    source/characters/character.cpp \
    $framework_flags \
    -o build/rpg-game

printf 'Built %s\n' "build/rpg-game"
