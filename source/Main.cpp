#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>

#include "ActionObject.h"
#include "BattleClass.h"
#include "GameItems.h"
#include "Inventory.h"
#include "SaveState.h"
#include "Sound.h"
#include "WorldMap.h"
#include "character.h"
#include "Gambling.h"

namespace {

int read_int_choice()
{
    int choice{};
    while (!(std::cin >> choice)) {
        if (std::cin.eof()) {
            return -1;
        }

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Please enter a number: ";
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return choice;
}

const char* item_type_label(InventoryItem::Type type)
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

const char* danger_label(int danger_level)
{
    switch (danger_level) {
    case 0:
        return "Safe";
    case 1:
        return "Low";
    case 2:
        return "Guarded";
    case 3:
        return "High";
    default:
        return "Deadly";
    }
}

void errorSound() { PlaySoundCue(SoundCue::Error); }
void magicSound() { PlaySoundCue(SoundCue::Magic); }
void menuSound() { PlaySoundCue(SoundCue::Menu); }
void attackSound() { PlaySoundCue(SoundCue::Attack); }
void endSound() { PlaySoundCue(SoundCue::End); }

void wait_for_enter()
{
    std::cout << "Press Enter to continue...";
    std::cout.flush();

    std::string ignored;
    std::getline(std::cin, ignored);
}

void fully_restore(Character& character)
{
    character.heal(character.get_max_health());
    character.restore_mana(character.get_max_mana());
    character.restore_stamina(character.get_max_stamina());
}

void sync_equipped_weapon(Character& player, const Inventory& inventory)
{
    player.set_attack(inventory.equipped_attack_bonus());
}

std::mt19937& game_rng()
{
    static std::mt19937 rng(std::random_device{}());
    return rng;
}

int random_int(int min_value, int max_value)
{
    std::uniform_int_distribution<int> dist(min_value, max_value);
    return dist(game_rng());
}

bool add_named_item(Inventory& inventory, const std::string& item_name)
{
    const InventoryItem* item = GameItems::FindByName(item_name);
    return item && inventory.try_add_item(*item);
}

void print_inventory_summary(const std::vector<ItemSummary>& summary,
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

void print_routes(RegionId region)
{
    const auto neighbors = GetConnectedRegions(region);
    std::cout << "Routes from here: ";
    for (size_t i = 0; i < neighbors.size(); ++i) {
        if (i > 0) {
            std::cout << ", ";
        }
        std::cout << RegionName(neighbors[i]);
    }
    std::cout << "\n";
}

void print_region_line(RegionId region, const AdventureState& adventure)
{
    const RegionData& data = GetRegionData(region);
    std::cout << "  - " << data.name;
    if (adventure.current_region == region) {
        std::cout << " [You are here]";
    } else if (HasVisited(adventure, region)) {
        std::cout << " [Visited]";
    }
    std::cout << " [" << danger_label(data.danger_level) << "]\n";
}

void print_world_map_and_journal(const AdventureState& adventure,
                                 const Character& hero,
                                 const Inventory& inventory)
{
    const RegionData& region = GetRegionData(adventure.current_region);

    std::cout << "\n=== WORLD MAP ===\n";
    std::cout << "Chapter: " << StoryStageName(adventure.story_stage) << "\n";
    std::cout << "Objective: " << StoryObjective(adventure) << "\n";
    std::cout << "Current location: " << region.name
              << " [" << danger_label(region.danger_level) << "]\n";
    std::cout << region.description << "\n";
    print_routes(adventure.current_region);

    std::cout << "\nNorth\n";
    print_region_line(RegionId::IceCourt, adventure);
    print_region_line(RegionId::Gelt, adventure);

    std::cout << "\nWestern Mainland\n";
    print_region_line(RegionId::NorthernWilds, adventure);
    print_region_line(RegionId::River, adventure);
    print_region_line(RegionId::Mudlands, adventure);
    print_region_line(RegionId::Channel, adventure);
    print_region_line(RegionId::Glade, adventure);
    print_region_line(RegionId::RuneMountains, adventure);
    print_region_line(RegionId::PatomicCity, adventure);
    print_region_line(RegionId::CentaurionPlaines, adventure);
    print_region_line(RegionId::SouthernExpanse, adventure);
    print_region_line(RegionId::ShrineOfTheWatchmaker, adventure);
	print_region_line(RegionId::Casino, adventure);

    std::cout << "\nEastern Reach\n";
    print_region_line(RegionId::EasternSea, adventure);
    print_region_line(RegionId::StormSpiralIsles, adventure);
    print_region_line(RegionId::EasternMountainChain, adventure);
    print_region_line(RegionId::BlinkeringIsle, adventure);

    std::cout << "\nParty Status\n";
    std::cout << "Health: " << hero.get_health() << "/" << hero.get_max_health()
              << " | Mana: " << hero.get_mana() << "/" << hero.get_max_mana()
              << " | Gold: " << hero.get_gold()
              << " | Inventory: " << inventory.size() << "/"
              << inventory.capacity() << "\n";
}

void manage_inventory(Inventory& inventory, Character& player)
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

        const int choice = read_int_choice();
        switch (choice) {
        case 1:
            if (summary.empty()) {
                std::cout << "Your inventory is empty.\n";
            } else {
                print_inventory_summary(summary, equipped);
            }
            break;
        case 2: {
            std::vector<ItemSummary> consumables;
            for (const auto& item : summary) {
                if (item.type == InventoryItem::Type::Consumable) {
                    consumables.push_back(item);
                }
            }

            if (consumables.empty()) {
                std::cout << "You have no consumables ready.\n";
                break;
            }

            print_inventory_summary(consumables, equipped);
            std::cout << "Select consumable to use (0 to cancel): ";
            const int selection = read_int_choice();
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
                magicSound();
            } else if (item_name == "Herb") {
                player.restore_mana(12);
                magicSound();
            } else {
                std::cout << "You cannot use " << item_name << " yet.\n";
                break;
            }

            inventory.remove_item_by_name(item_name);
            sync_equipped_weapon(player, inventory);

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
            std::vector<ItemSummary> weapons;
            for (const auto& item : summary) {
                if (item.type == InventoryItem::Type::Weapon) {
                    weapons.push_back(item);
                }
            }

            if (weapons.empty()) {
                std::cout << "No weapons to equip.\n";
                break;
            }

            print_inventory_summary(weapons, equipped);
            std::cout << "Select weapon to equip (0 to cancel): ";
            const int selection = read_int_choice();
            if (selection == 0) {
                break;
            }
            if (selection < 1 || selection > static_cast<int>(weapons.size())) {
                std::cout << "Invalid choice.\n";
                break;
            }

            const std::string& weapon_name = weapons[selection - 1].name;
            if (inventory.equip_weapon(weapon_name)) {
                sync_equipped_weapon(player, inventory);
                std::cout << "Equipped " << weapon_name << ".\n";
                menuSound();
            } else {
                std::cout << "Could not equip " << weapon_name << ".\n";
            }
            break;
        }
        case 4:
            inventory.unequip_weapon();
            sync_equipped_weapon(player, inventory);
            std::cout << "Weapon unequipped.\n";
            break;
        case 5:
            if (summary.empty()) {
                std::cout << "There is nothing to discard.\n";
                break;
            }
            print_inventory_summary(summary, equipped);
            std::cout << "Select item to discard (0 to cancel): ";
            {
                const int selection = read_int_choice();
                if (selection == 0) {
                    break;
                }
                if (selection < 1 || selection > static_cast<int>(summary.size())) {
                    std::cout << "Invalid choice.\n";
                    break;
                }

                const std::string& item_name = summary[selection - 1].name;
                if (inventory.remove_item_by_name(item_name)) {
                    sync_equipped_weapon(player, inventory);
                    std::cout << "Discarded one " << item_name << ".\n";
                } else {
                    std::cout << "Could not discard " << item_name << ".\n";
                }
            }
            break;
        case 6:
            in_menu = false;
            break;
        case -1:
            in_menu = false;
            break;
        default:
            errorSound();
            std::cout << "Invalid choice.\n";
            break;
        }
    }
}

bool travel_to_new_region(AdventureState& adventure)
{
    const RegionId current = adventure.current_region;
    const auto neighbors = GetConnectedRegions(current);

    std::cout << "\n=== TRAVEL ===\n";
    std::cout << "Current location: " << RegionName(current) << "\n";
    for (size_t i = 0; i < neighbors.size(); ++i) {
        const RegionData& destination = GetRegionData(neighbors[i]);
        std::cout << (i + 1) << ") " << destination.name
                  << " [" << danger_label(destination.danger_level) << "]\n";
    }
    std::cout << "0) Stay put\nChoice: ";

    const int choice = read_int_choice();
    if (choice == 0 || choice == -1) {
        return false;
    }
    if (choice < 1 || choice > static_cast<int>(neighbors.size())) {
        errorSound();
        std::cout << "That route is not available.\n";
        return false;
    }

    const RegionId destination = neighbors[choice - 1];
    const bool first_visit = !HasVisited(adventure, destination);
    adventure.current_region = destination;
    MarkVisited(adventure, destination);

    const RegionData& region = GetRegionData(destination);
    std::cout << "\nYou travel from " << RegionName(current)
              << " to " << region.name << ".\n";
    if (first_visit) {
        std::cout << "First impression: " << region.description << "\n";
    } else {
        std::cout << region.description << "\n";
    }
    print_routes(destination);

    return true;
}

struct EncounterSpec {
    std::string name;
    std::string opening_line;
    Character::Stats stats;
};

std::string random_enemy_name(RegionId region)
{
    switch (region) {
    case RegionId::IceCourt: {
        static const std::vector<std::string> names = {
            "Frostbound Exile", "Rime Wolf"
        };
        return names[random_int(0, static_cast<int>(names.size()) - 1)];
    }
    case RegionId::Gelt: {
        static const std::vector<std::string> names = {
            "Gelt Smuggler", "Harbor Knifehand"
        };
        return names[random_int(0, static_cast<int>(names.size()) - 1)];
    }
    case RegionId::NorthernWilds: {
        static const std::vector<std::string> names = {
            "Wildfang Stalker", "Pinecloak Hunter"
        };
        return names[random_int(0, static_cast<int>(names.size()) - 1)];
    }
    case RegionId::River: {
        static const std::vector<std::string> names = {
            "River Drake Whelp", "Reedway Bandit"
        };
        return names[random_int(0, static_cast<int>(names.size()) - 1)];
    }
    case RegionId::Mudlands: {
        static const std::vector<std::string> names = {
            "Bog Leech Alpha", "Mudlands Marauder"
        };
        return names[random_int(0, static_cast<int>(names.size()) - 1)];
    }
    case RegionId::Channel: {
        static const std::vector<std::string> names = {
            "Channel Cutthroat", "Bridge Trollkin"
        };
        return names[random_int(0, static_cast<int>(names.size()) - 1)];
    }
    case RegionId::Glade: {
        static const std::vector<std::string> names = {
            "Bramble Warden", "Hollow Deer"
        };
        return names[random_int(0, static_cast<int>(names.size()) - 1)];
    }
    case RegionId::RuneMountains: {
        static const std::vector<std::string> names = {
            "Rune-Touched Raider", "Stonecut Marauder"
        };
        return names[random_int(0, static_cast<int>(names.size()) - 1)];
    }
    case RegionId::PatomicCity: {
        static const std::vector<std::string> names = {
            "Clocktower Thief", "Dockside Ruffian"
        };
        return names[random_int(0, static_cast<int>(names.size()) - 1)];
    }
    case RegionId::CentaurionPlaines: {
        static const std::vector<std::string> names = {
            "Plains Skirmisher", "Hoofstep Ambusher"
        };
        return names[random_int(0, static_cast<int>(names.size()) - 1)];
    }
    case RegionId::SouthernExpanse: {
        static const std::vector<std::string> names = {
            "Dustglass Viper", "Coastline Reaver"
        };
        return names[random_int(0, static_cast<int>(names.size()) - 1)];
    }
    case RegionId::ShrineOfTheWatchmaker: {
        static const std::vector<std::string> names = {
            "Broken Sentinel", "Tide Wraith"
        };
        return names[random_int(0, static_cast<int>(names.size()) - 1)];
    }
    case RegionId::EasternSea: {
        static const std::vector<std::string> names = {
            "Saltbound Corsair", "Sea-Glass Reaver"
        };
        return names[random_int(0, static_cast<int>(names.size()) - 1)];
    }
    case RegionId::StormSpiralIsles: {
        static const std::vector<std::string> names = {
            "Spiral Cultist", "Stormwake Harrier"
        };
        return names[random_int(0, static_cast<int>(names.size()) - 1)];
    }
    case RegionId::EasternMountainChain: {
        static const std::vector<std::string> names = {
            "Storm Roc", "Cliffside Ravager"
        };
        return names[random_int(0, static_cast<int>(names.size()) - 1)];
    }
    case RegionId::BlinkeringIsle: {
        static const std::vector<std::string> names = {
            "Blinker Wisp", "Beacon Raider"
        };
        return names[random_int(0, static_cast<int>(names.size()) - 1)];
    }
    case RegionId::Count:
        break;
    }

    return "Wandering Foe";
}

EncounterSpec make_region_encounter(const AdventureState& adventure)
{
    const RegionData& region = GetRegionData(adventure.current_region);
    const int stage_bonus = static_cast<int>(adventure.story_stage);
    const int base_health = 42 + (region.danger_level * 18) + (stage_bonus * 8);
    const int stamina = 10 + (region.danger_level * 2);

    Character::Stats stats{};
    stats.max_health = base_health + random_int(0, 12);
    stats.health = stats.max_health;
    stats.max_mana = 0;
    stats.mana = 0;
    stats.max_stamina = stamina;
    stats.stamina = stamina;
    stats.actions = { 3, 4, 0, 0, 0, 0 };

    EncounterSpec encounter;
    encounter.name = random_enemy_name(adventure.current_region);
    encounter.opening_line =
        "Trouble finds you in " + std::string(region.name) +
        " as " + encounter.name + " steps into your path.";
    encounter.stats = stats;

    return encounter;
}

bool run_encounter(Character& hero,
                   Inventory& inventory,
                   const EncounterSpec& encounter)
{
    sync_equipped_weapon(hero, inventory);
    attackSound();

    std::cout << "\n" << encounter.opening_line << "\n";

    Character enemy(encounter.name, encounter.stats);
    std::vector<Character*> heroes{ &hero };
    std::vector<Character*> enemies{ &enemy };
    Battle battle(heroes, enemies, &inventory);

    return hero.is_alive() && !enemy.is_alive();
}

std::string exploration_item_for_region(RegionId region)
{
    switch (region) {
    case RegionId::RuneMountains:
    case RegionId::EasternMountainChain:
        return (random_int(1, 100) <= 35) ? "Iron Sword" : "Potion";
    case RegionId::CentaurionPlaines:
        return (random_int(1, 100) <= 35) ? "Training Dagger" : "Herb";
    case RegionId::SouthernExpanse:
    case RegionId::Glade:
        return (random_int(1, 100) <= 50) ? "Herb" : "Potion";
    default:
        return (random_int(1, 100) <= 70) ? "Potion" : "Herb";
    }
}

bool explore_region(AdventureState& adventure,
                    Character& hero,
                    Inventory& inventory)
{
    if (adventure.story_stage == StoryStage::DefendPatomic &&
        adventure.current_region == RegionId::PatomicCity) {
        EncounterSpec finale;
        finale.name = "Spiral Captain";
        finale.opening_line =
            "The square falls silent as the Spiral Captain tears open the storm above Patomic City's clocktower.";
        finale.stats.max_health = 150;
        finale.stats.health = 150;
        finale.stats.max_mana = 0;
        finale.stats.mana = 0;
        finale.stats.max_stamina = 16;
        finale.stats.stamina = 16;
        finale.stats.actions = { 4, 6, 0, 0, 0, 0 };

        const bool victory = run_encounter(hero, inventory, finale);
        if (!hero.is_alive()) {
            std::cout << "Patomic City's bells go quiet as you fall.\n";
            return false;
        }

        adventure.story_stage = StoryStage::Epilogue;
        magicSound();
        std::cout << "\nWith the Tideglass Sigil raised high, the city's ward catches the storm and bends it harmlessly out to sea.\n";
        std::cout << "Patomic City's towers answer one another again, and the old roads of the world feel open for the first time in years.\n";
        wait_for_enter();
        return victory;
    }

    const RegionData& region = GetRegionData(adventure.current_region);

    if (region.danger_level == 0) {
        if (adventure.current_region == RegionId::Casino) {
            Gambling::showMenu(hero.get_gold_ref());
            wait_for_enter();
            return true;
        }
        if (adventure.current_region == RegionId::PatomicCity) {
            const int gold_found = random_int(4, 9);
            hero.add_gold(gold_found);
            std::cout << "You walk the markets and pick up rumors, directions, and "
                      << gold_found << " gold in grateful tips.\n";
        } else {
            fully_restore(hero);
            magicSound();
            std::cout << "The shrine's stillness lets you breathe. Your strength returns.\n";
        }
        wait_for_enter();
        return true;
    }

    const int battle_chance = 20 + (region.danger_level * 15);
    const int event_roll = random_int(1, 100);

    if (event_roll <= battle_chance) {
        const EncounterSpec encounter = make_region_encounter(adventure);
        const bool victory = run_encounter(hero, inventory, encounter);
        if (!hero.is_alive()) {
            std::cout << "Your journey ends in " << region.name << ".\n";
            return false;
        }

        if (victory) {
            std::cout << "You survive the clash and keep moving.\n";
        }
        wait_for_enter();
        return true;
    }

    if (event_roll <= battle_chance + 40) {
        const int gold_found = random_int(5, 11) + region.danger_level;
        hero.add_gold(gold_found);
        std::cout << "While exploring " << region.name
                  << ", you find a hidden cache worth "
                  << gold_found << " gold.\n";
        wait_for_enter();
        return true;
    }

    const std::string item_name = exploration_item_for_region(adventure.current_region);
    if (add_named_item(inventory, item_name)) {
        std::cout << "You uncover " << item_name << " while exploring "
                  << region.name << ".\n";
    } else {
        const int gold_found = random_int(3, 7);
        hero.add_gold(gold_found);
        std::cout << "Your pack is full, so you settle for "
                  << gold_found << " gold instead.\n";
    }

    wait_for_enter();
    return true;
}

bool process_story_event(AdventureState& adventure,
                         Character& hero,
                         Inventory& inventory)
{
    switch (adventure.story_stage) {
    case StoryStage::Prologue:
        if (adventure.current_region != RegionId::PatomicCity) {
            return true;
        }

        magicSound();
        std::cout << "\n=== Prologue ===\n";
        std::cout << "Patomic City's harbor clocks skip an hour, then strike thirteen.\n";
        std::cout << "Keeper Elara catches you beneath the clocktower and begs for help before panic swallows the docks.\n";
        std::cout << "She says the old ward road can only be read from the Glade, where the standing stones still remember the Watchmaker's signs.\n";

        if (add_named_item(inventory, "Wooden Sword")) {
            inventory.equip_weapon("Wooden Sword");
            sync_equipped_weapon(hero, inventory);
            std::cout << "Elara shoves a Wooden Sword into your hands.\n";
        }
        if (add_named_item(inventory, "Potion")) {
            std::cout << "She adds a Potion to your satchel and points you toward the north road.\n";
        }

        adventure.story_stage = StoryStage::SeekGlade;
        wait_for_enter();
        return true;

    case StoryStage::SeekGlade:
        if (adventure.current_region != RegionId::Glade) {
            return true;
        }

        magicSound();
        std::cout << "\n=== The Glade ===\n";
        std::cout << "The standing stones in the Glade wake with a pale green pulse as you approach.\n";
        std::cout << "A grove-seer traces the spiral storms back to the Shrine of the Watchmaker in the Southern Expanse.\n";
        std::cout << "You copy the route, learn the shape of the Tideglass Sigil, and ready yourself for the southern road.\n";

        if (add_named_item(inventory, "Herb")) {
            std::cout << "The grove-seer presses an Herb into your palm for the journey.\n";
        }

        adventure.story_stage = StoryStage::SeekShrine;
        wait_for_enter();
        return true;

    case StoryStage::SeekShrine:
        if (adventure.current_region != RegionId::ShrineOfTheWatchmaker) {
            return true;
        }

        magicSound();
        std::cout << "\n=== Shrine Of The Watchmaker ===\n";
        std::cout << "At the shrine, dormant gears turn for the first time in years and reveal the Tideglass Sigil.\n";
        std::cout << "A mirrored vision shows Patomic City ringed by spiral stormclouds and a captain waiting to break the ward from within.\n";

        if (add_named_item(inventory, "Iron Sword")) {
            inventory.equip_weapon("Iron Sword");
            sync_equipped_weapon(hero, inventory);
            std::cout << "You claim an Iron Sword from the reliquary and belt it on at once.\n";
        }

        adventure.story_stage = StoryStage::ReturnToPatomic;
        wait_for_enter();
        return true;

    case StoryStage::ReturnToPatomic:
        if (adventure.current_region != RegionId::PatomicCity) {
            return true;
        }

        magicSound();
        std::cout << "\n=== Storm Over Patomic ===\n";
        std::cout << "You reach Patomic City ahead of the storm wall, but only barely.\n";
        std::cout << "Clockkeepers and dockhands rush to bar doors while Elara binds your wounds and begs you to confront the Spiral Captain.\n";
        fully_restore(hero);
        std::cout << "You are restored to full strength before the final push.\n";

        adventure.story_stage = StoryStage::DefendPatomic;
        wait_for_enter();
        return true;

    case StoryStage::DefendPatomic:
    case StoryStage::Epilogue:
        return true;
    }

    return true;
}

void print_main_menu(const AdventureState& adventure,
                     const Character& hero,
                     const Inventory& inventory)
{
    const RegionData& region = GetRegionData(adventure.current_region);

    std::cout << "\n=== ADVENTURE LOOP ===\n";
    std::cout << "Location: " << region.name
              << " [" << danger_label(region.danger_level) << "]\n";
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
              << "5) Manage inventory\n"
              << "6) Save journey\n"
              << "7) Load journey\n"
              << "8) Quit\n"
              << "Choice: ";
}

} // namespace

int main()
{
    if (!LoadDataBase()) {
        std::cout << "Warning: action data failed to load. Battle moves may be unavailable.\n";
    }

    Character::Stats heroStats{};
    heroStats.max_health = 120;
    heroStats.health = 120;
    heroStats.max_mana = 35;
    heroStats.mana = 35;
    heroStats.max_stamina = 18;
    heroStats.stamina = 18;
    heroStats.attack = 0;
    heroStats.actions = { 1, 2, 6, 5, 0, 0 };

    Character hero("Hero", heroStats);
    Inventory inventory;
    AdventureState adventure = CreateNewAdventure();

    while (true) {
        std::cout << "\n=== TITLE SCREEN ===\n";
        std::cout << "1) Start new journey\n"
                  << "2) Load journey\n"
                  << "3) Quit\n"
                  << "Choice: ";

        const int choice = read_int_choice();
        if (choice == 1) {
            menuSound();
            break;
        }

        if (choice == 2) {
            menuSound();
            const SaveResult result = LoadGameState(hero, inventory, adventure);
            std::cout << result.message << "\n";
            if (result.ok) {
                sync_equipped_weapon(hero, inventory);
                break;
            }
            wait_for_enter();
            continue;
        }

        if (choice == 3 || choice == -1) {
            endSound();
            std::cout << "Goodbye!\n";
            return 0;
        }

        errorSound();
        std::cout << "Invalid choice.\n";
    }

    bool running = true;
    bool player_survived = true;

    while (running) {
        if (!process_story_event(adventure, hero, inventory)) {
            player_survived = false;
            break;
        }

        print_main_menu(adventure, hero, inventory);
        const int choice = read_int_choice();

        switch (choice) {
        case 1:
            menuSound();
            print_world_map_and_journal(adventure, hero, inventory);
            wait_for_enter();
            break;
        case 2:
            menuSound();
            travel_to_new_region(adventure);
            break;
        case 3:
            menuSound();
            if (!explore_region(adventure, hero, inventory)) {
                player_survived = false;
                running = false;
            }
            break;
        case 4:
            magicSound();
            fully_restore(hero);
            std::cout << "You rest at " << RegionName(adventure.current_region)
                      << " and recover your strength.\n";
            wait_for_enter();
            break;
        case 5:
            menuSound();
            manage_inventory(inventory, hero);
            break;
        case 6: {
            menuSound();
            const SaveResult result = SaveGameState(hero, inventory, adventure);
            std::cout << result.message << "\n";
            wait_for_enter();
            break;
        }
        case 7: {
            menuSound();
            const SaveResult result = LoadGameState(hero, inventory, adventure);
            std::cout << result.message << "\n";
            if (result.ok) {
                sync_equipped_weapon(hero, inventory);
            }
            wait_for_enter();
            break;
        }
        case 8:
        case -1:
            endSound();
            running = false;
            break;
        default:
            errorSound();
            std::cout << "Invalid choice.\n";
            break;
        }

        if (!hero.is_alive()) {
            player_survived = false;
            running = false;
        }
    }

    if (!player_survived) {
        std::cout << "Your journey ends here.\n";
    }

    std::cout << "Goodbye!\n";
    return 0;
}
