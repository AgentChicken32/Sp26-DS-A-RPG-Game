#include <iostream>

#include "data/ActionObject.h"
#include "world/AdventureFlow.h"
#include "inventory/InventoryMenu.h"
#include "persistence/SaveState.h"
#include "platform/UiCommon.h"
#include "world/WorldMap.h"
#include "world/WorldUi.h"
#include "characters/character.h"

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

        const int choice = ReadIntChoice();
        if (choice == 1) {
            PlayMenuSound();
            break;
        }

        if (choice == 2) {
            PlayMenuSound();
            const SaveResult result = LoadGameState(hero, inventory, adventure);
            std::cout << result.message << "\n";
            if (result.ok) {
                SyncEquippedWeapon(hero, inventory);
                break;
            }
            WaitForEnter();
            continue;
        }

        if (choice == 3 || choice == -1) {
            PlayEndSound();
            std::cout << "Goodbye!\n";
            return 0;
        }

        PlayErrorSound();
        std::cout << "Invalid choice.\n";
    }

    bool running = true;
    bool player_survived = true;

    while (running) {
        if (!ProcessStoryEvent(adventure, hero, inventory)) {
            player_survived = false;
            break;
        }

        PrintMainMenu(adventure, hero, inventory);
        const int choice = ReadIntChoice();

        switch (choice) {
        case 1:
            PlayMenuSound();
            PrintWorldMapAndJournal(adventure, hero, inventory);
            WaitForEnter();
            break;
        case 2:
            PlayMenuSound();
            TravelToNewRegion(adventure);
            break;
        case 3:
            PlayMenuSound();
            if (!ExploreRegion(adventure, hero, inventory)) {
                player_survived = false;
                running = false;
            }
            break;
        case 4:
            PlayMagicSound();
            FullyRestore(hero);
            std::cout << "You rest at " << RegionName(adventure.current_region)
                      << " and recover your strength.\n";
            WaitForEnter();
            break;
        case 5:
            PlayMenuSound();
            VisitShop(adventure, hero, inventory);
            break;
        case 6:
            PlayMenuSound();
            ManageInventory(inventory, hero);
            break;
        case 7: {
            PlayMenuSound();
            const SaveResult result = SaveGameState(hero, inventory, adventure);
            std::cout << result.message << "\n";
            WaitForEnter();
            break;
        }
        case 8: {
            PlayMenuSound();
            const SaveResult result = LoadGameState(hero, inventory, adventure);
            std::cout << result.message << "\n";
            if (result.ok) {
                SyncEquippedWeapon(hero, inventory);
            }
            WaitForEnter();
            break;
        }
        case 9:
        case -1:
            PlayEndSound();
            running = false;
            break;
        default:
            PlayErrorSound();
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
