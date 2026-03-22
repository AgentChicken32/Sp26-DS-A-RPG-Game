#pragma once

#include <string>

#include "Inventory.h"
#include "character.h"

struct LootDropResult {
    bool awarded_item = false;
    bool awarded_gold = false;
    int gold_amount = 0;
    InventoryItem item;
    std::string message;
};

LootDropResult AwardBattleLoot(Character& hero,
                               Inventory& inventory,
                               int defeated_enemy_count);
