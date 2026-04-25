#include "persistence/SaveState.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <vector>

#include "inventory/GameItems.h"
#include "third_party/json.hpp"

namespace {

constexpr const char* kSaveMagic = "RPGSAVE";
constexpr int kSchemaVersion = 3;
constexpr const char* kSaveFileName = "save_slot_1.json";

const std::array<std::filesystem::path, 3> kPrimarySaveDirs = {
    "runtime/saves",
    "../runtime/saves",
    "../../runtime/saves"
};

const std::array<std::filesystem::path, 3> kLegacyReadSaveDirs = {
    "saves",
    "../saves",
    "../../saves"
};

std::string ToStatusName(StatusCondition status) {
    switch (status) {
    case StatusCondition::None:
        return "None";
    case StatusCondition::Poison:
        return "Poison";
    case StatusCondition::Burn:
        return "Burn";
    }
    return "None";
}

StatusCondition ParseStatusName(const std::string& name) {
    if (name == "Poison") return StatusCondition::Poison;
    if (name == "Burn") return StatusCondition::Burn;
    return StatusCondition::None;
}

std::string ToItemTypeName(InventoryItem::Type type) {
    switch (type) {
    case InventoryItem::Type::Weapon:
        return "Weapon";
    case InventoryItem::Type::Armor:
        return "Armor";
    case InventoryItem::Type::Consumable:
        return "Consumable";
    case InventoryItem::Type::Misc:
    default:
        return "Misc";
    }
}

InventoryItem::Type ParseItemTypeName(const std::string& name) {
    if (name == "Weapon") return InventoryItem::Type::Weapon;
    if (name == "Armor") return InventoryItem::Type::Armor;
    if (name == "Consumable") return InventoryItem::Type::Consumable;
    return InventoryItem::Type::Misc;
}

std::string TimestampUtcNow() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t raw = std::chrono::system_clock::to_time_t(now);

    std::tm utc_tm{};
#if defined(_WIN32)
    gmtime_s(&utc_tm, &raw);
#else
    gmtime_r(&raw, &utc_tm);
#endif

    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &utc_tm);
    return buffer;
}

std::optional<std::filesystem::path> ResolveSavePathForRead() {
    for (const auto& saveDir : kPrimarySaveDirs) {
        const std::filesystem::path candidate = saveDir / kSaveFileName;
        if (std::filesystem::exists(candidate)) {
            return candidate;
        }
    }

    for (const auto& saveDir : kLegacyReadSaveDirs) {
        const std::filesystem::path candidate = saveDir / kSaveFileName;
        if (std::filesystem::exists(candidate)) {
            return candidate;
        }
    }

    return std::nullopt;
}

std::optional<std::filesystem::path> ResolveSavePathForWrite() {
    for (const auto& saveDir : kPrimarySaveDirs) {
        std::error_code ec;
        std::filesystem::create_directories(saveDir, ec);
        if (!ec) {
            return saveDir / kSaveFileName;
        }
    }
    return std::nullopt;
}

nlohmann::json SerializeHero(const Character& hero) {
    const Character::Stats& stats = hero.get_stats();

    nlohmann::json heroData;
    heroData["name"] = hero.get_name();

    nlohmann::json statsData;
    statsData["max_health"] = stats.max_health;
    statsData["health"] = stats.health;
    statsData["max_mana"] = stats.max_mana;
    statsData["mana"] = stats.mana;
    statsData["max_stamina"] = stats.max_stamina;
    statsData["stamina"] = stats.stamina;
    statsData["attack"] = stats.attack;
    statsData["gold"] = stats.gold;
    statsData["status_condition"] = ToStatusName(stats.statusCondition);
    statsData["actions"] = std::vector<int>(stats.actions.begin(), stats.actions.end());

    heroData["stats"] = statsData;
    return heroData;
}

nlohmann::json SerializeInventory(const Inventory& inventory) {
    nlohmann::json inventoryData;
    inventoryData["equipped_weapon_name"] =
        inventory.equipped_weapon() ? inventory.equipped_weapon()->name : "";

    nlohmann::json items = nlohmann::json::array();
    for (const auto& item : inventory.dump_items()) {
        nlohmann::json entry;
        entry["name"] = item.name;
        entry["type"] = ToItemTypeName(item.type);
        entry["attack_bonus"] = item.attack_bonus;
        entry["value"] = item.value;
        entry["description"] = item.description;
        items.push_back(entry);
    }
    inventoryData["items"] = items;
    return inventoryData;
}

nlohmann::json SerializeShops(const AdventureState& adventure) {
    nlohmann::json shops = nlohmann::json::array();

    for (RegionId id : GetAllRegions()) {
        const std::size_t region = static_cast<std::size_t>(id);

        nlohmann::json shopData;
        shopData["region"] = RegionName(id);
        shopData["refresh_stage"] = adventure.shop_refresh_stage[region];

        nlohmann::json stock = nlohmann::json::array();
        for (std::size_t slot = 0; slot < kShopSlotCount; ++slot) {
            stock.push_back(adventure.shop_stock[region][slot]);
        }
        shopData["stock"] = stock;

        shops.push_back(shopData);
    }

    return shops;
}

nlohmann::json SerializeAdventure(const AdventureState& adventure) {
    nlohmann::json adventureData;
    adventureData["current_region"] = RegionName(adventure.current_region);
    adventureData["story_stage"] = StoryStageName(adventure.story_stage);

    nlohmann::json visited = nlohmann::json::array();
    for (RegionId id : GetAllRegions()) {
        if (HasVisited(adventure, id)) {
            visited.push_back(RegionName(id));
        }
    }
    adventureData["visited_regions"] = visited;
    adventureData["shops"] = SerializeShops(adventure);

    return adventureData;
}

Character::Stats ParseStats(const nlohmann::json& statsData,
                            const Character::Stats& fallback) {
    Character::Stats stats = fallback;

    stats.max_health = statsData.value("max_health", fallback.max_health);
    stats.health = statsData.value("health", fallback.health);
    stats.max_mana = statsData.value("max_mana", fallback.max_mana);
    stats.mana = statsData.value("mana", fallback.mana);
    stats.max_stamina = statsData.value("max_stamina", fallback.max_stamina);
    stats.stamina = statsData.value("stamina", fallback.stamina);
    stats.attack = statsData.value("attack", fallback.attack);
    stats.gold = statsData.value("gold", fallback.gold);
    stats.statusCondition =
        ParseStatusName(statsData.value("status_condition", std::string("None")));

    if (statsData.contains("actions") && statsData["actions"].is_array()) {
        const auto& actionsData = statsData["actions"];
        for (size_t i = 0; i < stats.actions.size() && i < actionsData.size(); ++i) {
            if (actionsData[i].is_number_integer()) {
                stats.actions[i] = actionsData[i].get<int>();
            }
        }
    }

    return stats;
}

std::vector<InventoryItem> ParseInventoryItems(const nlohmann::json& itemsData) {
    std::vector<InventoryItem> items;
    if (!itemsData.is_array()) {
        return items;
    }

    for (const auto& itemData : itemsData) {
        if (!itemData.is_object()) {
            continue;
        }

        InventoryItem item;
        item.name = itemData.value("name", std::string("Unknown Item"));
        if (const InventoryItem* catalogItem = GameItems::FindByName(item.name)) {
            item = *catalogItem;
        } else {
            item.type = ParseItemTypeName(itemData.value("type", std::string("Misc")));
            item.attack_bonus = itemData.value("attack_bonus", 0);
            item.value = itemData.value("value", 0);
            item.description = itemData.value("description", std::string(""));
        }
        items.push_back(item);
    }

    return items;
}

void ParseShopState(const nlohmann::json& shopsData, AdventureState& adventure) {
    if (!shopsData.is_array()) {
        return;
    }

    for (const auto& shopData : shopsData) {
        if (!shopData.is_object()) {
            continue;
        }

        RegionId region{};
        if (!TryParseRegionId(shopData.value("region", std::string("")), region)) {
            continue;
        }

        const std::size_t regionIndex = static_cast<std::size_t>(region);
        adventure.shop_refresh_stage[regionIndex] =
            shopData.value("refresh_stage", -1);

        const nlohmann::json stockData =
            shopData.value("stock", nlohmann::json::array());
        const RegionShopData& shop = GetRegionShopData(region);
        for (std::size_t slot = 0; slot < kShopSlotCount; ++slot) {
            int quantity = shop.stock[slot].base_quantity;
            if (stockData.is_array() && slot < stockData.size() &&
                stockData[slot].is_number_integer()) {
                quantity = stockData[slot].get<int>();
            }

            adventure.shop_stock[regionIndex][slot] =
                std::clamp(quantity, 0, shop.stock[slot].base_quantity);
        }
    }
}

AdventureState ParseAdventureState(const nlohmann::json& adventureData) {
    AdventureState adventure = CreateNewAdventure();

    if (adventureData.is_object()) {
        RegionId region = adventure.current_region;
        if (TryParseRegionId(
                adventureData.value("current_region", std::string(RegionName(region))),
                region)) {
            adventure.current_region = region;
        }

        StoryStage stage = adventure.story_stage;
        if (TryParseStoryStage(
                adventureData.value("story_stage",
                                    std::string(StoryStageName(stage))),
                stage)) {
            adventure.story_stage = stage;
        }

        adventure.visited.fill(false);
        if (adventureData.contains("visited_regions") &&
            adventureData["visited_regions"].is_array()) {
            for (const auto& regionValue : adventureData["visited_regions"]) {
                if (!regionValue.is_string()) {
                    continue;
                }

                RegionId visitedRegion{};
                if (TryParseRegionId(regionValue.get<std::string>(), visitedRegion)) {
                    MarkVisited(adventure, visitedRegion);
                }
            }
        }

        if (adventureData.contains("shops")) {
            ParseShopState(adventureData["shops"], adventure);
        }
    }

    MarkVisited(adventure, adventure.current_region);
    for (RegionId id : GetAllRegions()) {
        EnsureShopStockFresh(adventure, id);
    }
    return adventure;
}

SaveResult MigrateSaveToLatest(nlohmann::json& saveData) {
    int version = saveData.value("schema_version", 0);

    if (version > kSchemaVersion) {
        return {false, "Save file is from a newer version of the game."};
    }

    if (!saveData.contains("state") || !saveData["state"].is_object()) {
        return {false, "Save file is missing a valid state object."};
    }

    while (version < kSchemaVersion) {
        if (version == 0) {
            saveData["schema_version"] = 1;
            version = 1;
            continue;
        }

        if (version == 1) {
            if (!saveData["state"].contains("adventure") ||
                !saveData["state"]["adventure"].is_object()) {
                saveData["state"]["adventure"] = SerializeAdventure(CreateNewAdventure());
            }

            saveData["schema_version"] = 2;
            version = 2;
            continue;
        }

        if (version == 2) {
            saveData["schema_version"] = 3;
            version = 3;
            continue;
        }

        return {false, "Could not migrate this save to the latest schema."};
    }

    return {true, ""};
}

} // namespace

SaveResult SaveGameState(const Character& hero,
                         const Inventory& inventory,
                         const AdventureState& adventure) {
    const auto targetPathOpt = ResolveSavePathForWrite();
    if (!targetPathOpt.has_value()) {
        return {false, "Save failed: could not create a save directory."};
    }
    const std::filesystem::path targetPath = targetPathOpt.value();
    const std::filesystem::path tempPath = targetPath.string() + ".tmp";
    const std::filesystem::path backupPath = targetPath.string() + ".bak";

    nlohmann::json root;
    root["magic"] = kSaveMagic;
    root["schema_version"] = kSchemaVersion;
    root["saved_at_utc"] = TimestampUtcNow();
    root["game_build"] = "main";

    nlohmann::json state;
    state["hero"] = SerializeHero(hero);
    state["inventory"] = SerializeInventory(inventory);
    state["adventure"] = SerializeAdventure(adventure);
    root["state"] = state;

    std::ofstream out(tempPath, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        return {false, "Save failed: could not open temporary save file."};
    }
    out << root.dump(2);
    out.close();

    std::error_code ec;
    if (std::filesystem::exists(targetPath, ec)) {
        std::filesystem::copy_file(
            targetPath,
            backupPath,
            std::filesystem::copy_options::overwrite_existing,
            ec);
        ec.clear();
    }

    std::filesystem::rename(tempPath, targetPath, ec);
    if (ec) {
        std::filesystem::remove(tempPath, ec);
        return {false, std::string("Save failed: could not finalize save file (") + ec.message() + ")."};
    }

    return {true, "Save complete. Your progress is tucked away safely."};
}

SaveResult LoadGameState(Character& hero,
                         Inventory& inventory,
                         AdventureState& adventure) {
    const auto sourcePathOpt = ResolveSavePathForRead();
    if (!sourcePathOpt.has_value()) {
        return {false, "Load failed: no save file found yet."};
    }
    const std::filesystem::path sourcePath = sourcePathOpt.value();

    std::ifstream file(sourcePath);
    if (!file.is_open()) {
        return {false, "Load failed: could not open save file."};
    }

    nlohmann::json root;
    try {
        file >> root;
    } catch (const std::exception& ex) {
        return {false, std::string("Load failed: save file is invalid JSON (") + ex.what() + ")."};
    }

    if (root.value("magic", std::string("")) != kSaveMagic) {
        return {false, "Load failed: this file is not a recognized RPG save."};
    }

    SaveResult migration = MigrateSaveToLatest(root);
    if (!migration.ok) {
        return migration;
    }

    const nlohmann::json& state = root["state"];
    if (!state.contains("hero") || !state["hero"].is_object()) {
        return {false, "Load failed: save is missing hero data."};
    }
    if (!state.contains("inventory") || !state["inventory"].is_object()) {
        return {false, "Load failed: save is missing inventory data."};
    }

    const nlohmann::json& heroData = state["hero"];
    const nlohmann::json& heroStatsData = heroData.value("stats", nlohmann::json::object());
    const std::string heroName = heroData.value("name", hero.get_name());
    Character::Stats loadedStats = ParseStats(heroStatsData, hero.get_stats());

    const nlohmann::json& inventoryData = state["inventory"];
    std::vector<InventoryItem> loadedItems =
        ParseInventoryItems(inventoryData.value("items", nlohmann::json::array()));
    const std::string equippedWeaponName =
        inventoryData.value("equipped_weapon_name", std::string(""));
    const AdventureState loadedAdventure =
        ParseAdventureState(state.value("adventure", nlohmann::json::object()));

    hero = Character(heroName, loadedStats);
    inventory.load_items(loadedItems, equippedWeaponName);
    hero.set_attack(inventory.equipped_attack_bonus());
    adventure = loadedAdventure;

    return {true, "Load complete. Party and inventory are back in action."};
}
