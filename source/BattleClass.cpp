#include "BattleClass.h"
#include "character.h"

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

// ------------------------------------------------------------
// Basic Implementation of a player attack function
void Battle::BasicPlayerAttack(Character* npc) {
    int input = 0;
    bool loop = false;

    do {
        loop = false;

        cout << "*-------------------------------------------------------*\n";
        cout << "Who do you want to wack?\n";

        // Print enemies + BACK
        for (int i = 0; i < static_cast<int>(enemies.size()); i++) {
            cout << (i + 1) << ". " << enemies[i]->get_name()
                 << "[HP:" << enemies[i]->get_health() << "] ";
        }
        cout << (enemies.size() + 1) << ". BACK\n";

        cin >> input;

        // BACK
        if (input == static_cast<int>(enemies.size()) + 1) {
            return; // go back to PlayerTurn menu
        }

        // Invalid range
        if (input < 1 || input > static_cast<int>(enemies.size())) {
            cout << "*-------------------------------------------------------*\n";
            cout << "Invalid choice. Try again.\n";
            loop = true;
            continue;
        }

        Character* target = enemies[input - 1];

        // Already down
        if (!target->is_alive()) {
            cout << "*-------------------------------------------------------*\n";
            cout << "That target is already down. Pick someone else (or BACK).\n";
            loop = true;
            continue;
        }

        // Attack
        target->take_damage(20);
        cout << "*-------------------------------------------------------*\n";
        cout << "You hit " << target->get_name() << "!\n";
        cout << "It took 20 damage!\n";

        if (!target->is_alive()) {
            cout << target->get_name() << " is defeated!\n";
        }

    } while (loop);
}

// ------------------------------------------------------------
// Basic Implementation of player magic function
void Battle::BasicPlayerMagic(Character* npc) {
    int input = 0;
    bool loop = false;

    do {
        loop = false;

        cout << "*-------------------------------------------------------*\n";
        cout << "Who do you want to blast with a fireball?\n";

        for (int i = 0; i < static_cast<int>(enemies.size()); i++) {
            cout << (i + 1) << ". " << enemies[i]->get_name()
                 << "[HP:" << enemies[i]->get_health() << "] ";
        }
        cout << (enemies.size() + 1) << ". BACK\n";

        cin >> input;

        // BACK
        if (input == static_cast<int>(enemies.size()) + 1) {
            return;
        }

        // Invalid range
        if (input < 1 || input > static_cast<int>(enemies.size())) {
            cout << "*-------------------------------------------------------*\n";
            cout << "Invalid choice. Try again.\n";
            loop = true;
            continue;
        }

        Character* target = enemies[input - 1];

        // Already down
        if (!target->is_alive()) {
            cout << "*-------------------------------------------------------*\n";
            cout << "That target is already down. Pick someone else (or BACK).\n";
            loop = true;
            continue;
        }

        // Optional: mana check (if you want it)
        // if (npc->get_mana() < 10) {
        //     cout << "*-------------------------------------------------------*\n";
        //     cout << "Not enough mana!\n";
        //     loop = true;
        //     continue;
        // }

        target->take_damage(30);
        npc->spend_mana(10);

        cout << "*-------------------------------------------------------*\n";
        cout << "You hit " << target->get_name() << " with a fireball!\n";
        cout << "It took 30 damage!\n";

        if (!target->is_alive()) {
            cout << target->get_name() << " is defeated!\n";
        }

    } while (loop);
}

// ------------------------------------------------------------

Battle::Battle(vector<Character*> good, vector<Character*> evil)
    : Scene("Battle")
{
    heroes = good;
    enemies = evil;

    DecideTurnOrder();
    Setup();
}

void Battle::Setup() {
    if (turnCounter == -1) {
        cout << "Stand guard, enemies appeared!\n";
        turnCounter = 0;
    }

    int result = 0;

    while ((result = CheckForWinLoss()) == 0) {
        if (turnOrder.empty()) break; // extra safety

        Character* actor = turnOrder[turnCounter];

        bool is_enemy = (std::find(enemies.begin(), enemies.end(), actor) != enemies.end());

        if (is_enemy) {
            EnemyTurn(actor);
        } else {
            PlayerTurn(actor);
        }

        // advance turn counter with wrap-around
        if (!turnOrder.empty()) {
            turnCounter++;
            if (turnCounter >= static_cast<int>(turnOrder.size())) {
                turnCounter = 0;
            }
        }
    }

    if (result == 1) {
        cout << "You win!\n";
    } else if (result == 2) {
        cout << "You lose!\n";
    }
}

void Battle::DecideTurnOrder() {
    turnOrder.clear();

    turnOrder = heroes;
    turnOrder.insert(turnOrder.end(), enemies.begin(), enemies.end());

    std::sort(turnOrder.begin(), turnOrder.end(),
        [](Character* a, Character* b) {
            return a->get_stamina() > b->get_stamina();
        });

    turnCounter = 0;
}

void Battle::PlayerTurn(Character* npc) {
    cout << "*-------------------------------------------------------*\n";
    cout << "It is " << npc->get_name() << "'s turn!\n";
    cout << "Health: " << npc->get_health() << " Mana: " << npc->get_mana() << "\n";

    int input = 0;
    bool loop = false;

    do {
        loop = false;

        cout << R"(
*-------------------------------------------------------*
Choose an action!
1. ATTACK    2. MAGIC    3. ITEMS    4. ESCAPE
)" << endl;

        cin >> input;

        switch (input) {
        case 1:
            BasicPlayerAttack(npc);
            break;
        case 2:
            BasicPlayerMagic(npc);
            break;
        case 3:
            AccessInventory();
            loop = true;
            break;
        case 4:
            cout << R"(
*-------------------------------------------------------*
You tried to run away, but it failed!
)" << endl;
            loop = true;
            break;
        default:
            loop = true;
        }

    } while (loop);
}

void Battle::EnemyTurn(Character* npc) {
    if (heroes.empty()) return;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> chooseTarget(0, static_cast<int>(heroes.size()) - 1);
    std::uniform_int_distribution<int> damage(20, 30);

    int attempts = 0;
    int idx = chooseTarget(gen);

    while (!heroes[idx]->is_alive() && attempts < 100) {
        idx = chooseTarget(gen);
        attempts++;
    }
    if (!heroes[idx]->is_alive()) return;

    const int dmg = damage(gen);
    heroes[idx]->take_damage(dmg);

    cout << "*-------------------------------------------------------*\n";
    cout << npc->get_name() << " wacked " << heroes[idx]->get_name() << "!\n";
    cout << heroes[idx]->get_name() << " has " << heroes[idx]->get_health() << " health left!\n";
}

int Battle::CheckForWinLoss() {
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](Character* c) { return !c || !c->is_alive(); }),
        enemies.end()
    );

    heroes.erase(
        std::remove_if(heroes.begin(), heroes.end(),
            [](Character* c) { return !c || !c->is_alive(); }),
        heroes.end()
    );

    turnOrder.erase(
        std::remove_if(turnOrder.begin(), turnOrder.end(),
            [](Character* c) { return !c || !c->is_alive(); }),
        turnOrder.end()
    );

    if (!turnOrder.empty() && turnCounter >= static_cast<int>(turnOrder.size())) {
        turnCounter = 0;
    }

    if (enemies.empty()) return 1;
    if (heroes.empty())  return 2;
    return 0;
}

void Battle::AccessInventory() {
    cout << "*-------------------------------------------------------*\n";
    cout << "There is nothing in your bag...\n";
}

void Battle::MenuOptions() {
    return;
}