#pragma once

#include <string>

#include "Inventory.h"
#include "WorldMap.h"
#include "character.h"

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
