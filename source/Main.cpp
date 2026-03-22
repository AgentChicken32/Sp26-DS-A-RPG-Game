#include <iostream>
#include <limits>
#include <vector>
#include <string>

#include "Inventory.h"
#include "GameItems.h"
#include "character.h"

#include "BattleClass.h"

#include "ActionObject.h"
#include "SaveState.h"

#include "DialogueDatabase.h"

// Reads an int choice safely (clears bad input).
static int read_int_choice()
{
    int choice{};
    while (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Please enter a number: ";
    }
    return choice;
}

static int read_positive_int(const std::string& prompt)
{
    int value = 0;
    do {
        std::cout << prompt;
        value = read_int_choice();
    } while (value <= 0);
    return value;
}

static const char* item_type_label(InventoryItem::Type type)
{
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

static void print_item_catalog(const std::vector<InventoryItem>& catalog)
{
    std::cout << "\n--- Item Catalog ---\n";
    for (size_t i = 0; i < catalog.size(); ++i) {
        const auto& item = catalog[i];
        std::cout << (i + 1) << ") " << item.name
                  << " [" << item_type_label(item.type) << "]";
        if (item.type == InventoryItem::Type::Weapon) {
            std::cout << " (+" << item.attack_bonus << " atk)";
        }
        std::cout << "\n";
    }
}

static void print_inventory_summary(const std::vector<ItemSummary>& summary,
                                    const InventoryItem* equipped)
{
    std::cout << "\n--- Inventory ---\n";
    for (size_t i = 0; i < summary.size(); ++i) {
        const auto& entry = summary[i];
        std::cout << (i + 1) << ") " << entry.name
                  << " x" << entry.count
                  << " [" << item_type_label(entry.type) << "]";
        if (entry.type == InventoryItem::Type::Weapon) {
            std::cout << " (+" << entry.attack_bonus << " atk)";
        }
        if (equipped && entry.name == equipped->name) {
            std::cout << " [EQUIPPED]";
        }
        std::cout << "\n";
    }
}

static void manage_inventory(Inventory& inventory, Character& player)
{
    const auto& itemCatalog = GameItems::Catalog();

    bool in_menu = true;
    while (in_menu) {
        const InventoryItem* equipped = inventory.equipped_weapon();
        std::cout << "\n=== INVENTORY MENU ===\n";
        std::cout << "Gold: " << player.get_gold() << "\n";
        std::cout << "Slots: " << inventory.size() << "/" << inventory.capacity()
                  << " (" << inventory.remaining_capacity() << " free)\n";
        if (equipped) {
            std::cout << "Equipped weapon: " << equipped->name
                      << " (+" << equipped->attack_bonus << " atk)\n";
        } else {
            std::cout << "Equipped weapon: None\n";
        }
        std::cout << "1) View items\n"
                  << "2) Add item\n"
                  << "3) Remove item\n"
                  << "4) Equip weapon\n"
                  << "5) Unequip weapon\n"
                  << "6) Back\n"
                  << "Choice: ";

        const int choice = read_int_choice();
        switch (choice) {
        case 1: {
            const auto summary = inventory.summarize();
            if (summary.empty()) {
                std::cout << "Your inventory is empty.\n";
            } else {
                print_inventory_summary(summary, equipped);
            }
            break;
        }
        case 2: {
            if (inventory.is_full()) {
                std::cout << "Your inventory is full.\n";
                break;
            }

            print_item_catalog(itemCatalog);
            std::cout << "Select item to add (0 to cancel): ";
            int selection = read_int_choice();
            if (selection == 0) {
                break;
            }
            if (selection < 1 || selection > static_cast<int>(itemCatalog.size())) {
                std::cout << "Invalid choice.\n";
                break;
            }
            const int quantity = read_positive_int("Quantity: ");
            const InventoryItem& item = itemCatalog[selection - 1];
            int addedCount = 0;
            for (int i = 0; i < quantity; ++i) {
                if (!inventory.try_add_item(item)) {
                    break;
                }
                ++addedCount;
            }

            if (addedCount == quantity) {
                std::cout << "Added " << quantity << "x " << item.name << ".\n";
            } else if (addedCount > 0) {
                std::cout << "Added " << addedCount << "x " << item.name
                          << ". Inventory is now full.\n";
            } else {
                std::cout << "Could not add " << item.name
                          << ". Inventory is full.\n";
            }
            break;
        }
        case 3: {
            auto summary = inventory.summarize();
            if (summary.empty()) {
                std::cout << "Your inventory is empty.\n";
                break;
            }
            print_inventory_summary(summary, equipped);
            std::cout << "Select item to remove (0 to cancel): ";
            int selection = read_int_choice();
            if (selection == 0) {
                break;
            }
            if (selection < 1 || selection > static_cast<int>(summary.size())) {
                std::cout << "Invalid choice.\n";
                break;
            }
            const std::string& name = summary[selection - 1].name;
            if (inventory.remove_item_by_name(name)) {
                std::cout << "Removed one " << name << ".\n";
            } else {
                std::cout << "Could not remove " << name << ".\n";
            }
            player.set_attack(inventory.equipped_attack_bonus());
            break;
        }
        case 4: {
            auto summary = inventory.summarize();
            std::vector<ItemSummary> weapons;
            for (const auto& entry : summary) {
                if (entry.type == InventoryItem::Type::Weapon) {
                    weapons.push_back(entry);
                }
            }
            if (weapons.empty()) {
                std::cout << "No weapons to equip.\n";
                break;
            }
            print_inventory_summary(weapons, equipped);
            std::cout << "Select weapon to equip (0 to cancel): ";
            int selection = read_int_choice();
            if (selection == 0) {
                break;
            }
            if (selection < 1 || selection > static_cast<int>(weapons.size())) {
                std::cout << "Invalid choice.\n";
                break;
            }
            const std::string& name = weapons[selection - 1].name;
            if (inventory.equip_weapon(name)) {
                player.set_attack(inventory.equipped_attack_bonus());
                std::cout << "Equipped " << name << ".\n";
            } else {
                std::cout << "Could not equip " << name << ".\n";
            }
            break;
        }
        case 5:
            inventory.unequip_weapon();
            player.set_attack(inventory.equipped_attack_bonus());
            std::cout << "Weapon unequipped.\n";
            break;
        case 6:
            in_menu = false;
            break;
        default:
            std::cout << "Invalid choice.\n";
            break;
        }
    }
}
// Simple dialogue runner
static void run_dialogue(const std::string& groupName) {
    auto script = GetDialogue(groupName);
    if (script.empty()) {
        std::cout << "No dialogue found for group '" << groupName << "'.\n";
        return;
    }

    int currentId = 1; // assume each group starts at node id 1

    while (true) {
        auto it = script.find(currentId);
        if (it == script.end()) {
            std::cout << "[Dialogue ended: missing node " << currentId << "]\n";
            break;
        }

        TextNode node = it->second;
        std::cout << "\n" << node.text << "\n";

        // If this node is a choice, show options and let the player pick
        if (node.type == Type::Choice && !node.options.empty()) {
            for (std::size_t i = 0; i < node.options.size(); ++i) {
                const DialogueOption& option = node.options[i];
                std::cout << (i + 1) << ") " << option.text << "\n";
            }

            std::cout << "Choice: ";
            int choiceIndex = read_int_choice();

            if (choiceIndex < 1 || choiceIndex > static_cast<int>(node.options.size())) {
                std::cout << "Invalid choice, ending conversation.\n";
                break;
            }

            const DialogueOption& chosenOpt = node.options[choiceIndex - 1];

            if (chosenOpt.next == 0) {
                std::cout << "[Dialogue ended]\n";
                break;
            }

            currentId = chosenOpt.next;
            continue;
        }

        // Statement node: go to its next, or end if next == 0
        if (node.next == 0) {
            std::cout << "[Dialogue ended]\n";
            break;
        }

        currentId = node.next;
    }
}


int main()
{
    //EXTREMELY IMPORTANT: loads every action for battle use
    if (!LoadDataBase()) {
        std::cout << "Warning: action data failed to load. Battle moves may be unavailable.\n";
    }

    if (!LoadDialogueDatabase()) {
        std::cout << "Warning: dialogue data failed to load. Dialogue may be unavailable." << std::endl;
    }
    
    Character::Stats heroStats{};
    heroStats.max_health  = 100;
    heroStats.health      = 100;
    heroStats.max_mana    = 20;
    heroStats.mana        = 20;
    heroStats.max_stamina = 15;
    heroStats.stamina     = 15;
    heroStats.attack      = 0;
    heroStats.actions = { 1, 2, 3, 6, 5, 69 };

    Character hero("Hero", heroStats);
    Inventory inventory;

    bool running = true;

    while (running) {
        std::cout << "\n=== RPG TESTER MENU ===\n"
                  << "Gold: " << hero.get_gold()
                  << " | Inventory: " << inventory.size() << "/"
                  << inventory.capacity() << "\n"
                  << "1) Return to menu\n"
                  << "2) Manage inventory\n"
                  << "3) Start battle (test)\n"
                  << "4) Save progress\n"
                  << "5) Load progress\n"
                  << "6) Talk (Conversation1)\n"
                  << "7) Talk (Conversation2)\n"
                  << "8) Quit\n"
                  << "Choice: ";


        const int choice = read_int_choice();

        switch (choice) {

        case 1:
            // No-op for now (keeps looping)
            break;

        case 2:
            manage_inventory(inventory, hero);
            break;

        case 3: {
            std::cout << "\n[DEBUG] Starting battle...\n";

            Character::Stats enemyStats{};
            enemyStats.max_health  = 60;
            enemyStats.health      = 60;
            enemyStats.max_mana    = 0;
            enemyStats.mana        = 0;
            enemyStats.max_stamina = 10;
            enemyStats.stamina     = 10;

            Character villain("Goblin", enemyStats);

            std::vector<Character*> heroes{ &hero };
            std::vector<Character*> enemies{ &villain };

            Battle battle(heroes, enemies, &inventory);

            std::cout << "[DEBUG] Battle ended. Returning to menu...\n";
            break;
        }
        case 4: {
            SaveResult result = SaveGameState(hero, inventory);
            std::cout << result.message << "\n";
            break;
        }
        case 5: {
            SaveResult result = LoadGameState(hero, inventory);
            std::cout << result.message << "\n";
            break;
        }
        case 6: {
            run_dialogue("Conversation1");
            break;
        }
        case 7: {
            run_dialogue("Conversation2");
            break;
        }
        case 8: {
            running = false;
            break;
        }
        default:
            std::cout << "Invalid choice.\n";
            break;
        }
    }

    std::cout << "Goodbye!\n";
    return 0;
}
