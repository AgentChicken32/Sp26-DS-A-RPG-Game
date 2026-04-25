#include "battle/LootTable.h"

#include <algorithm>
#include <array>
#include <random>

#include "inventory/GameItems.h"

namespace {

struct WeightedLootEntry {
    const char* item_name = "";
    int weight = 0;
};

std::mt19937& LootRng() {
    static std::mt19937 rng(std::random_device{}());
    return rng;
}

int RandomInt(int min_value, int max_value) {
    std::uniform_int_distribution<int> dist(min_value, max_value);
    return dist(LootRng());
}

const InventoryItem& ChooseLootItem() {
    static const std::array<WeightedLootEntry, 5> kLootTable = {{
        {"Potion", 32},
        {"Herb", 24},
        {"Training Dagger", 18},
        {"Wooden Sword", 16},
        {"Iron Sword", 10}
    }};

    int total_weight = 0;
    for (const auto& entry : kLootTable) {
        total_weight += entry.weight;
    }

    int roll = RandomInt(1, total_weight);
    for (const auto& entry : kLootTable) {
        roll -= entry.weight;
        if (roll <= 0) {
            if (const InventoryItem* item = GameItems::FindByName(entry.item_name)) {
                return *item;
            }
            break;
        }
    }

    return GameItems::Catalog().front();
}

} // namespace

LootDropResult AwardBattleLoot(Character& hero,
                               Inventory& inventory,
                               int defeated_enemy_count) {
    LootDropResult result;
    const int safe_enemy_count = std::max(1, defeated_enemy_count);
    const int item_drop_chance = std::min(75, 35 + (safe_enemy_count * 10));
    const bool can_carry_item = !inventory.is_full();

    if (can_carry_item && RandomInt(1, 100) <= item_drop_chance) {
        const InventoryItem& item = ChooseLootItem();
        if (inventory.try_add_item(item)) {
            result.awarded_item = true;
            result.item = item;
            result.message =
                "Loot drop: " + item.name + " was added to your inventory.";
            return result;
        }
    }

    const int gold_amount = RandomInt(6, 12) * safe_enemy_count;
    hero.add_gold(gold_amount);

    result.awarded_gold = true;
    result.gold_amount = gold_amount;
    if (can_carry_item) {
        result.message =
            "Loot drop: you found " + std::to_string(gold_amount) + " gold.";
    } else {
        result.message =
            "Inventory full. You pocketed " + std::to_string(gold_amount) +
            " gold instead.";
    }

    return result;
}
