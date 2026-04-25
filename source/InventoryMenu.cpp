#include "InventoryMenu.h"

#include <iostream>
#include <vector>

#include "UiCommon.h"

namespace {

void PrintInventorySummary(const std::vector<ItemSummary>& summary,
                           const InventoryItem* equipped)
{
    std::cout << "\n--- Inventory ---\n";
    for (std::size_t i = 0; i < summary.size(); ++i) {
        const auto& entry = summary[i];
        std::cout << (i + 1) << ") " << entry.name
                  << " x" << entry.count
                  << " [" << ItemTypeLabel(entry.type) << "]";
        if (entry.type == InventoryItem::Type::Weapon) {
            std::cout << " (+" << entry.attack_bonus << " atk)";
        }
        if (equipped && entry.name == equipped->name) {
            std::cout << " [EQUIPPED]";
        }
        std::cout << "\n";
    }
}

std::vector<ItemSummary> FilterSummaryByType(const std::vector<ItemSummary>& summary,
                                             InventoryItem::Type type)
{
    std::vector<ItemSummary> filtered;
    for (const auto& item : summary) {
        if (item.type == type) {
            filtered.push_back(item);
        }
    }
    return filtered;
}

} // namespace

void ManageInventory(Inventory& inventory, Character& player)
{
    bool in_menu = true;

    while (in_menu) {
        const InventoryItem* equipped = inventory.equipped_weapon();
        const auto summary = inventory.summarize();

        std::cout << "\n=== INVENTORY ===\n";
        std::cout << "Health: " << player.get_health() << "/" << player.get_max_health()
                  << " | Mana: " << player.get_mana() << "/" << player.get_max_mana()
                  << " | Gold: " << player.get_gold() << "\n";
        std::cout << "Slots: " << inventory.size() << "/" << inventory.capacity()
                  << " (" << inventory.remaining_capacity() << " free)\n";
        std::cout << "Equipped weapon: "
                  << (equipped ? equipped->name : "None");
        if (equipped) {
            std::cout << " (+" << equipped->attack_bonus << " atk)";
        }
        std::cout << "\n";
        std::cout << "1) View items\n"
                  << "2) Use consumable\n"
                  << "3) Equip weapon\n"
                  << "4) Unequip weapon\n"
                  << "5) Discard item\n"
                  << "6) Back\n"
                  << "Choice: ";

        const int choice = ReadIntChoice();
        switch (choice) {
        case 1:
            if (summary.empty()) {
                std::cout << "Your inventory is empty.\n";
            } else {
                PrintInventorySummary(summary, equipped);
            }
            break;
        case 2: {
            const auto consumables =
                FilterSummaryByType(summary, InventoryItem::Type::Consumable);

            if (consumables.empty()) {
                std::cout << "You have no consumables ready.\n";
                break;
            }

            PrintInventorySummary(consumables, equipped);
            std::cout << "Select consumable to use (0 to cancel): ";
            const int selection = ReadIntChoice();
            if (selection == 0) {
                break;
            }
            if (selection < 1 || selection > static_cast<int>(consumables.size())) {
                std::cout << "Invalid choice.\n";
                break;
            }

            const std::string& item_name = consumables[selection - 1].name;
            const int health_before = player.get_health();
            const int mana_before = player.get_mana();

            if (item_name == "Potion") {
                player.heal(35);
                PlayMagicSound();
            } else if (item_name == "Herb") {
                player.restore_mana(12);
                PlayMagicSound();
            } else {
                std::cout << "You cannot use " << item_name << " yet.\n";
                break;
            }

            inventory.remove_item_by_name(item_name);
            SyncEquippedWeapon(player, inventory);

            const int health_restored = player.get_health() - health_before;
            const int mana_restored = player.get_mana() - mana_before;
            std::cout << "You use " << item_name << ". ";
            if (health_restored > 0) {
                std::cout << "Recovered " << health_restored << " health.";
            } else if (mana_restored > 0) {
                std::cout << "Recovered " << mana_restored << " mana.";
            } else {
                std::cout << "It settles your nerves, even if the numbers do not change.";
            }
            std::cout << "\n";
            break;
        }
        case 3: {
            const auto weapons =
                FilterSummaryByType(summary, InventoryItem::Type::Weapon);

            if (weapons.empty()) {
                std::cout << "No weapons to equip.\n";
                break;
            }

            PrintInventorySummary(weapons, equipped);
            std::cout << "Select weapon to equip (0 to cancel): ";
            const int selection = ReadIntChoice();
            if (selection == 0) {
                break;
            }
            if (selection < 1 || selection > static_cast<int>(weapons.size())) {
                std::cout << "Invalid choice.\n";
                break;
            }

            const std::string& weapon_name = weapons[selection - 1].name;
            if (inventory.equip_weapon(weapon_name)) {
                SyncEquippedWeapon(player, inventory);
                std::cout << "Equipped " << weapon_name << ".\n";
                PlayMenuSound();
            } else {
                std::cout << "Could not equip " << weapon_name << ".\n";
            }
            break;
        }
        case 4:
            inventory.unequip_weapon();
            SyncEquippedWeapon(player, inventory);
            std::cout << "Weapon unequipped.\n";
            break;
        case 5:
            if (summary.empty()) {
                std::cout << "There is nothing to discard.\n";
                break;
            }

            PrintInventorySummary(summary, equipped);
            std::cout << "Select item to discard (0 to cancel): ";
            {
                const int selection = ReadIntChoice();
                if (selection == 0) {
                    break;
                }
                if (selection < 1 || selection > static_cast<int>(summary.size())) {
                    std::cout << "Invalid choice.\n";
                    break;
                }

                const std::string& item_name = summary[selection - 1].name;
                if (inventory.remove_item_by_name(item_name)) {
                    SyncEquippedWeapon(player, inventory);
                    std::cout << "Discarded one " << item_name << ".\n";
                } else {
                    std::cout << "Could not discard " << item_name << ".\n";
                }
            }
            break;
        case 6:
        case -1:
            in_menu = false;
            break;
        default:
            PlayErrorSound();
            std::cout << "Invalid choice.\n";
            break;
        }
    }
}
