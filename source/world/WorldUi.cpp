#include "world/WorldUi.h"

#include <iostream>

#include "inventory/GameItems.h"
#include "platform/UiCommon.h"

namespace {

std::size_t RegionIndex(RegionId region)
{
    return static_cast<std::size_t>(region);
}

void PrintRoutes(RegionId region)
{
    const auto neighbors = GetConnectedRegions(region);
    std::cout << "Routes from here: ";
    for (std::size_t i = 0; i < neighbors.size(); ++i) {
        if (i > 0) {
            std::cout << ", ";
        }
        std::cout << RegionName(neighbors[i]);
    }
    std::cout << "\n";
}

void PrintRegionLine(RegionId region, const AdventureState& adventure)
{
    const RegionData& data = GetRegionData(region);
    std::cout << "  - " << data.name;
    if (adventure.current_region == region) {
        std::cout << " [You are here]";
    } else if (HasVisited(adventure, region)) {
        std::cout << " [Visited]";
    }
    std::cout << " [" << DangerLabel(data.danger_level) << "]\n";
}

} // namespace

void PrintWorldMapAndJournal(const AdventureState& adventure,
                             const Character& hero,
                             const Inventory& inventory)
{
    const RegionData& region = GetRegionData(adventure.current_region);

    std::cout << "\n=== WORLD MAP ===\n";
    std::cout << "Chapter: " << StoryStageName(adventure.story_stage) << "\n";
    std::cout << "Objective: " << StoryObjective(adventure) << "\n";
    std::cout << "Current location: " << region.name
              << " [" << DangerLabel(region.danger_level) << "]\n";
    std::cout << region.description << "\n";
    PrintRoutes(adventure.current_region);
    std::cout << "Local shop: "
              << GetRegionShopData(adventure.current_region).name << "\n";

    for (RegionSection section : GetRegionSections()) {
        std::cout << "\n" << RegionSectionName(section) << "\n";
        for (RegionId region_id : GetAllRegions()) {
            if (GetRegionData(region_id).section == section) {
                PrintRegionLine(region_id, adventure);
            }
        }
    }

    std::cout << "\nParty Status\n";
    std::cout << "Health: " << hero.get_health() << "/" << hero.get_max_health()
              << " | Mana: " << hero.get_mana() << "/" << hero.get_max_mana()
              << " | Gold: " << hero.get_gold()
              << " | Inventory: " << inventory.size() << "/"
              << inventory.capacity() << "\n";
}

void VisitShop(AdventureState& adventure,
               Character& hero,
               Inventory& inventory)
{
    const RegionId region_id = adventure.current_region;
    EnsureShopStockFresh(adventure, region_id);

    const RegionData& region = GetRegionData(region_id);
    const RegionShopData& shop = GetRegionShopData(region_id);
    auto& stock_counts = adventure.shop_stock[RegionIndex(region_id)];

    bool in_shop = true;
    while (in_shop) {
        std::cout << "\n=== " << shop.name << " ===\n";
        std::cout << "Location: " << region.name << "\n";
        std::cout << "Gold: " << hero.get_gold()
                  << " | Inventory: " << inventory.size() << "/"
                  << inventory.capacity() << "\n";

        for (std::size_t i = 0; i < shop.stock.size(); ++i) {
            const ShopStockEntry& offer = shop.stock[i];
            const InventoryItem* item = GameItems::FindByName(offer.item_name);

            std::cout << (i + 1) << ") " << offer.item_name
                      << " - " << offer.price << " gold"
                      << " | Stock: " << stock_counts[i];
            if (item) {
                std::cout << " [" << ItemTypeLabel(item->type) << "]";
                if (item->type == InventoryItem::Type::Weapon) {
                    std::cout << " (+" << item->attack_bonus << " atk)";
                }
                if (!item->description.empty()) {
                    std::cout << " - " << item->description;
                }
            }
            std::cout << "\n";
        }

        std::cout << "0) Leave shop\nChoice: ";
        const int choice = ReadIntChoice();
        if (choice == 0 || choice == -1) {
            in_shop = false;
            continue;
        }
        if (choice < 1 || choice > static_cast<int>(shop.stock.size())) {
            PlayErrorSound();
            std::cout << "That is not on the counter.\n";
            continue;
        }

        const std::size_t slot = static_cast<std::size_t>(choice - 1);
        const ShopStockEntry& offer = shop.stock[slot];
        const InventoryItem* item = GameItems::FindByName(offer.item_name);

        if (!item) {
            PlayErrorSound();
            std::cout << "The shopkeeper cannot find that item.\n";
            continue;
        }
        if (stock_counts[slot] <= 0) {
            PlayErrorSound();
            std::cout << offer.item_name << " is sold out until the story moves on.\n";
            continue;
        }
        if (inventory.is_full()) {
            PlayErrorSound();
            std::cout << "Your pack is full. Make room before buying more.\n";
            continue;
        }
        if (!hero.spend_gold(offer.price)) {
            PlayErrorSound();
            std::cout << "You need " << offer.price << " gold for "
                      << offer.item_name << ".\n";
            continue;
        }
        if (!inventory.try_add_item(*item)) {
            hero.add_gold(offer.price);
            PlayErrorSound();
            std::cout << "Your pack is full. The shopkeeper returns your gold.\n";
            continue;
        }

        --stock_counts[slot];
        PlayMenuSound();
        std::cout << "Bought " << offer.item_name << " for "
                  << offer.price << " gold.\n";
    }
}

void PrintMainMenu(const AdventureState& adventure,
                   const Character& hero,
                   const Inventory& inventory)
{
    const RegionData& region = GetRegionData(adventure.current_region);

    std::cout << "\n=== ADVENTURE LOOP ===\n";
    std::cout << "Location: " << region.name
              << " [" << DangerLabel(region.danger_level) << "]\n";
    std::cout << "Chapter: " << StoryStageName(adventure.story_stage) << "\n";
    std::cout << "Objective: " << StoryObjective(adventure) << "\n";
    std::cout << "Health: " << hero.get_health() << "/" << hero.get_max_health()
              << " | Mana: " << hero.get_mana() << "/" << hero.get_max_mana()
              << " | Gold: " << hero.get_gold()
              << " | Inventory: " << inventory.size() << "/"
              << inventory.capacity() << "\n";
    std::cout << "1) View world map and journal\n"
              << "2) Travel\n"
              << "3) Explore current region\n"
              << "4) Rest\n"
              << "5) Visit local shop\n"
              << "6) Manage inventory\n"
              << "7) Save journey\n"
              << "8) Load journey\n"
              << "9) Quit\n"
              << "Choice: ";
}
