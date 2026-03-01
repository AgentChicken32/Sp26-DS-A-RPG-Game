#pragma once

#include <string>

#include "Inventory.h"
#include "character.h"

struct SaveResult {
    bool ok = false;
    std::string message;
};

SaveResult SaveGameState(const Character& hero, const Inventory& inventory);
SaveResult LoadGameState(Character& hero, Inventory& inventory);
