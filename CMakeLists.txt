cmake_minimum_required(VERSION 3.20)

project(rpg_game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(rpg-game
    source/data/ActionObject.cpp
    source/world/AdventureFlow.cpp
    source/battle/BattleClass.cpp
    source/world/DialogueTree.cpp
    source/inventory/GameItems.cpp
    source/inventory/Inventory.cpp
    source/inventory/InventoryMenu.cpp
    source/battle/LootTable.cpp
    source/app/Main.cpp
    source/persistence/SaveState.cpp
    source/battle/SceneClass.cpp
    source/platform/Sound.cpp
    source/platform/UiCommon.cpp
    source/world/WorldUi.cpp
    source/world/WorldMap.cpp
    source/characters/character.cpp
    source/world/Gambling.cpp
)

target_include_directories(rpg-game PRIVATE source)

if(APPLE)
    find_library(AUDIOTOOLBOX_FRAMEWORK AudioToolbox REQUIRED)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation REQUIRED)
    target_link_libraries(rpg-game PRIVATE
        ${AUDIOTOOLBOX_FRAMEWORK}
        ${COREFOUNDATION_FRAMEWORK}
    )
endif()
