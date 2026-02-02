#include <iostream>
#include <limits>
#include <vector>

#include "character.h"

// Compatibility alias: BattleClass.* currently uses `Character*` (lowercase),
// while Character.h defines `Character`.
using Character = Character;

#include "BattleClass.h"

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

int main()
{
    bool running = true;

    while (running) {
        std::cout << "\n=== RPG TESTER MENU ===\n"
                  << "1) Return to menu\n"
                  << "2) Start battle (test)\n"
                  << "3) Quit\n"
                  << "Choice: ";

        const int choice = read_int_choice();

        switch (choice) {

        case 1:
            // No-op for now (keeps looping)
            break;

        case 2: {
            std::cout << "\n[DEBUG] Starting battle...\n";

            Character::Stats heroStats{};
            heroStats.max_health  = 100;
            heroStats.health      = 100;
            heroStats.max_mana    = 20;
            heroStats.mana        = 20;
            heroStats.max_stamina = 15;
            heroStats.stamina     = 15;

            Character::Stats enemyStats{};
            enemyStats.max_health  = 60;
            enemyStats.health      = 60;
            enemyStats.max_mana    = 0;
            enemyStats.mana        = 0;
            enemyStats.max_stamina = 10;
            enemyStats.stamina     = 10;

            Character hero("Hero", heroStats);
            Character villain("Goblin", enemyStats);

            std::vector<Character*> heroes{ &hero };
            std::vector<Character*> enemies{ &villain };

            Battle battle(heroes, enemies);

            std::cout << "[DEBUG] Battle ended. Returning to menu...\n";
            break;
        }
        case 3:
            running = false;
            break;

        default:
            std::cout << "Invalid choice.\n";
            break;
        }
    }

    std::cout << "Goodbye!\n";
    return 0;
}