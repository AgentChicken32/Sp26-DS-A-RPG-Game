cmake_minimum_required(VERSION 3.20)

project(rpg_game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(rpg-game
    source/ActionObject.cpp
    source/BattleClass.cpp
    source/DialogueTree.cpp
    source/GameItems.cpp
    source/Inventory.cpp
    source/LootTable.cpp
    source/Main.cpp
    source/RPG_T_Menu.cpp
    source/SaveState.cpp
    source/SceneClass.cpp
    source/Sound.cpp
    source/WorldMap.cpp
    source/character.cpp
 "source/Gambling.h" "source/Gambling.cpp")

if(APPLE)
    find_library(AUDIOTOOLBOX_FRAMEWORK AudioToolbox REQUIRED)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation REQUIRED)
    target_link_libraries(rpg-game PRIVATE
        ${AUDIOTOOLBOX_FRAMEWORK}
        ${COREFOUNDATION_FRAMEWORK}
    )
endif()
