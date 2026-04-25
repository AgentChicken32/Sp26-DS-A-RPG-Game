#include "AdventureFlow.h"

#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "BattleClass.h"
#include "DialogueTree.h"
#include "Gambling.h"
#include "GameItems.h"
#include "UiCommon.h"

namespace {

std::mt19937& GameRng()
{
    static std::mt19937 rng(std::random_device{}());
    return rng;
}

int RandomInt(int min_value, int max_value)
{
    std::uniform_int_distribution<int> dist(min_value, max_value);
    return dist(GameRng());
}

bool AddNamedItem(Inventory& inventory, const std::string& item_name)
{
    const InventoryItem* item = GameItems::FindByName(item_name);
    return item && inventory.try_add_item(*item);
}

void AdvanceStory(AdventureState& adventure, StoryStage next_stage)
{
    if (adventure.story_stage == next_stage) {
        return;
    }

    adventure.story_stage = next_stage;
    RefreshAllShopStock(adventure);
    std::cout << "News travels fast. Every region's shop refreshes its shelves.\n";
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

void RunDialogueTree(const DialogueTree& dialogue)
{
    if (!dialogue.root) {
        return;
    }

    PlayMenuSound();
    std::cout << "\n=== Conversation: " << dialogue.npc_name << " ===\n";
    std::cout << dialogue.encounter_text << "\n";

    const DialogueNode* current = dialogue.root.get();
    while (current) {
        std::cout << "\n" << current->speaker << ": " << current->line << "\n";

        if (current->choices.empty()) {
            std::cout << "The conversation ends.\n";
            return;
        }

        for (std::size_t i = 0; i < current->choices.size(); ++i) {
            std::cout << (i + 1) << ") " << current->choices[i].text << "\n";
        }
        std::cout << "0) End conversation\nChoice: ";

        const int choice = ReadIntChoice();
        if (choice == 0 || choice == -1) {
            std::cout << "You end the conversation.\n";
            return;
        }
        if (choice < 1 || choice > static_cast<int>(current->choices.size())) {
            PlayErrorSound();
            std::cout << "Invalid choice.\n";
            continue;
        }

        PlayMenuSound();
        current = current->choices[choice - 1].next.get();
    }
}

void RunRandomNpcDialogue(RegionId region)
{
    DialogueTree dialogue =
        CreateRandomNpcDialogueTree(region, RandomInt(0, 100000));
    RunDialogueTree(dialogue);
}

struct EncounterSpec {
    std::string name;
    std::string opening_line;
    Character::Stats stats;
};

EncounterSpec MakeRegionEncounter(const AdventureState& adventure)
{
    const RegionData& region = GetRegionData(adventure.current_region);
    const int stage_bonus = static_cast<int>(adventure.story_stage);
    const int base_health = 42 + (region.danger_level * 18) + (stage_bonus * 8);
    const int stamina = 10 + (region.danger_level * 2);

    Character::Stats stats{};
    stats.max_health = base_health + RandomInt(0, 12);
    stats.health = stats.max_health;
    stats.max_mana = 0;
    stats.mana = 0;
    stats.max_stamina = stamina;
    stats.stamina = stamina;
    stats.actions = { 3, 4, 0, 0, 0, 0 };

    EncounterSpec encounter;
    const auto& enemy_names = region.enemy_names;
    encounter.name =
        enemy_names[RandomInt(0, static_cast<int>(enemy_names.size()) - 1)];
    encounter.opening_line =
        "Trouble finds you in " + std::string(region.name) +
        " as " + encounter.name + " steps into your path.";
    encounter.stats = stats;

    return encounter;
}

bool RunEncounter(Character& hero,
                  Inventory& inventory,
                  const EncounterSpec& encounter)
{
    SyncEquippedWeapon(hero, inventory);
    PlayAttackSound();

    std::cout << "\n" << encounter.opening_line << "\n";

    Character enemy(encounter.name, encounter.stats);
    std::vector<Character*> heroes{ &hero };
    std::vector<Character*> enemies{ &enemy };
    Battle battle(heroes, enemies, &inventory);

    return hero.is_alive() && !enemy.is_alive();
}

std::string ExplorationItemForRegion(const RegionData& region)
{
    const ExplorationItemProfile& items = region.exploration_items;
    return (RandomInt(1, 100) <= items.first_item_chance)
        ? items.first_item
        : items.second_item;
}

} // namespace

bool TravelToNewRegion(AdventureState& adventure)
{
    const RegionId current = adventure.current_region;
    const auto neighbors = GetConnectedRegions(current);

    std::cout << "\n=== TRAVEL ===\n";
    std::cout << "Current location: " << RegionName(current) << "\n";
    for (std::size_t i = 0; i < neighbors.size(); ++i) {
        const RegionData& destination = GetRegionData(neighbors[i]);
        std::cout << (i + 1) << ") " << destination.name
                  << " [" << DangerLabel(destination.danger_level) << "]\n";
    }
    std::cout << "0) Stay put\nChoice: ";

    const int choice = ReadIntChoice();
    if (choice == 0 || choice == -1) {
        return false;
    }
    if (choice < 1 || choice > static_cast<int>(neighbors.size())) {
        PlayErrorSound();
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
    PrintRoutes(destination);

    return true;
}

bool ExploreRegion(AdventureState& adventure,
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

        const bool victory = RunEncounter(hero, inventory, finale);
        if (!hero.is_alive()) {
            std::cout << "Patomic City's bells go quiet as you fall.\n";
            return false;
        }

        AdvanceStory(adventure, StoryStage::Epilogue);
        PlayMagicSound();
        std::cout << "\nWith the Tideglass Sigil raised high, the city's ward catches the storm and bends it harmlessly out to sea.\n";
        std::cout << "Patomic City's towers answer one another again, and the old roads of the world feel open for the first time in years.\n";
        WaitForEnter();
        return victory;
    }

    const RegionData& region = GetRegionData(adventure.current_region);

    if (region.danger_level == 0) {
        if (adventure.current_region == RegionId::Casino) {
            Gambling::showMenu(hero.get_gold_ref());
            WaitForEnter();
            return true;
        }

        const int event_roll = RandomInt(1, 100);

        if (event_roll <= 45) {
            RunRandomNpcDialogue(adventure.current_region);
        } else if (adventure.current_region == RegionId::PatomicCity) {
            const int gold_found = RandomInt(4, 9);
            hero.add_gold(gold_found);
            std::cout << "You walk the markets and pick up rumors, directions, and "
                      << gold_found << " gold in grateful tips.\n";
        } else if (adventure.current_region == RegionId::ShrineOfTheWatchmaker) {
            FullyRestore(hero);
            PlayMagicSound();
            std::cout << "The shrine's stillness lets you breathe. Your strength returns.\n";
        } else {
            const int gold_found = RandomInt(3, 8);
            hero.add_gold(gold_found);
            std::cout << "You take a quiet loop through " << region.name
                      << " and pocket " << gold_found
                      << " gold from a lucky errand.\n";
        }
        WaitForEnter();
        return true;
    }

    const int battle_chance = 20 + (region.danger_level * 15);
    const int dialogue_chance = 20;
    const int gold_chance = 35;
    const int event_roll = RandomInt(1, 100);

    if (event_roll <= battle_chance) {
        const EncounterSpec encounter = MakeRegionEncounter(adventure);
        const bool victory = RunEncounter(hero, inventory, encounter);
        if (!hero.is_alive()) {
            std::cout << "Your journey ends in " << region.name << ".\n";
            return false;
        }

        if (victory) {
            std::cout << "You survive the clash and keep moving.\n";
        }
        WaitForEnter();
        return true;
    }

    if (event_roll <= battle_chance + dialogue_chance) {
        RunRandomNpcDialogue(adventure.current_region);
        WaitForEnter();
        return true;
    }

    if (event_roll <= battle_chance + dialogue_chance + gold_chance) {
        const int gold_found = RandomInt(5, 11) + region.danger_level;
        hero.add_gold(gold_found);
        std::cout << "While exploring " << region.name
                  << ", you find a hidden cache worth "
                  << gold_found << " gold.\n";
        WaitForEnter();
        return true;
    }

    const std::string item_name = ExplorationItemForRegion(region);
    if (AddNamedItem(inventory, item_name)) {
        std::cout << "You uncover " << item_name << " while exploring "
                  << region.name << ".\n";
    } else {
        const int gold_found = RandomInt(3, 7);
        hero.add_gold(gold_found);
        std::cout << "Your pack is full, so you settle for "
                  << gold_found << " gold instead.\n";
    }

    WaitForEnter();
    return true;
}

bool ProcessStoryEvent(AdventureState& adventure,
                       Character& hero,
                       Inventory& inventory)
{
    switch (adventure.story_stage) {
    case StoryStage::Prologue:
        if (adventure.current_region != RegionId::PatomicCity) {
            return true;
        }

        PlayMagicSound();
        std::cout << "\n=== Prologue ===\n";
        std::cout << "Patomic City's harbor clocks skip an hour, then strike thirteen.\n";
        std::cout << "Keeper Elara catches you beneath the clocktower and begs for help before panic swallows the docks.\n";
        std::cout << "She says the old ward road can only be read from the Glade, where the standing stones still remember the Watchmaker's signs.\n";

        if (AddNamedItem(inventory, "Wooden Sword")) {
            inventory.equip_weapon("Wooden Sword");
            SyncEquippedWeapon(hero, inventory);
            std::cout << "Elara shoves a Wooden Sword into your hands.\n";
        }
        if (AddNamedItem(inventory, "Potion")) {
            std::cout << "She adds a Potion to your satchel and points you toward the north road.\n";
        }

        AdvanceStory(adventure, StoryStage::SeekGlade);
        WaitForEnter();
        return true;

    case StoryStage::SeekGlade:
        if (adventure.current_region != RegionId::Glade) {
            return true;
        }

        PlayMagicSound();
        std::cout << "\n=== The Glade ===\n";
        std::cout << "The standing stones in the Glade wake with a pale green pulse as you approach.\n";
        std::cout << "A grove-seer traces the spiral storms back to the Shrine of the Watchmaker in the Southern Expanse.\n";
        std::cout << "You copy the route, learn the shape of the Tideglass Sigil, and ready yourself for the southern road.\n";

        if (AddNamedItem(inventory, "Herb")) {
            std::cout << "The grove-seer presses an Herb into your palm for the journey.\n";
        }

        AdvanceStory(adventure, StoryStage::SeekShrine);
        WaitForEnter();
        return true;

    case StoryStage::SeekShrine:
        if (adventure.current_region != RegionId::ShrineOfTheWatchmaker) {
            return true;
        }

        PlayMagicSound();
        std::cout << "\n=== Shrine Of The Watchmaker ===\n";
        std::cout << "At the shrine, dormant gears turn for the first time in years and reveal the Tideglass Sigil.\n";
        std::cout << "A mirrored vision shows Patomic City ringed by spiral stormclouds and a captain waiting to break the ward from within.\n";

        if (AddNamedItem(inventory, "Iron Sword")) {
            inventory.equip_weapon("Iron Sword");
            SyncEquippedWeapon(hero, inventory);
            std::cout << "You claim an Iron Sword from the reliquary and belt it on at once.\n";
        }

        AdvanceStory(adventure, StoryStage::ReturnToPatomic);
        WaitForEnter();
        return true;

    case StoryStage::ReturnToPatomic:
        if (adventure.current_region != RegionId::PatomicCity) {
            return true;
        }

        PlayMagicSound();
        std::cout << "\n=== Storm Over Patomic ===\n";
        std::cout << "You reach Patomic City ahead of the storm wall, but only barely.\n";
        std::cout << "Clockkeepers and dockhands rush to bar doors while Elara binds your wounds and begs you to confront the Spiral Captain.\n";
        FullyRestore(hero);
        std::cout << "You are restored to full strength before the final push.\n";

        AdvanceStory(adventure, StoryStage::DefendPatomic);
        WaitForEnter();
        return true;

    case StoryStage::DefendPatomic:
    case StoryStage::Epilogue:
        return true;
    }

    return true;
}
