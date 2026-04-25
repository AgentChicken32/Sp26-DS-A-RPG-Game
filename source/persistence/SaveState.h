#pragma once

#include <string>

#include "inventory/Inventory.h"
#include "world/WorldMap.h"
#include "characters/character.h"

struct SaveResult {
    bool ok = false;
    std::string message;
};

SaveResult SaveGameState(const Character& hero,
                         const Inventory& inventory,
                         const AdventureState& adventure);
SaveResult LoadGameState(Character& hero,
                         Inventory& inventory,
                         AdventureState& adventure);
