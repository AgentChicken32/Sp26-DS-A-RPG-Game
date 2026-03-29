#include "BattleClass.h"
#include "character.h"
#include "LootTable.h"
#include "Sound.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

using namespace std;

static int read_int_choice() {
    int choice{};
    while (!(cin >> choice)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Please enter a number: ";
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return choice;
}

static Character* choose_enemy_target(vector<Character*>& enemies) {
    vector<Character*> alive;
    for (Character* enemy : enemies) {
        if (enemy && enemy->is_alive()) {
            alive.push_back(enemy);
        }
    }

    if (alive.empty()) {
        return nullptr;
    }
    if (alive.size() == 1) {
        return alive[0];
    }

    while (true) {
        cout << "*-------------------------------------------------------*\n";
        cout << "Pick a target:\n";
        for (int i = 0; i < static_cast<int>(alive.size()); ++i) {
            cout << (i + 1) << ". " << alive[i]->get_name()
                 << "[HP:" << alive[i]->get_health() << "] ";
        }
        cout << (alive.size() + 1) << ". BACK\n";

        const int input = read_int_choice();
        if (input == static_cast<int>(alive.size()) + 1) {
            return nullptr;
        }
        if (input >= 1 && input <= static_cast<int>(alive.size())) {
            return alive[input - 1];
        }

        PlaySoundCue(SoundCue::Error);
        cout << "Invalid input! Please try again.\n";
    }
}

// ------------------------------------------------------------
// Basic Implementation of a player attack function
bool Battle::BasicPlayerAttack(Character* npc) {
    int input = 0;
    bool loop = false;

    const int base_damage = 20;
    const int bonus_damage = std::max(0, npc->get_attack());
    const int total_damage = base_damage + bonus_damage;

    do {
        loop = false;

        cout << "*-------------------------------------------------------*\n";
        cout << "Who do you want to whack?\n";

        // Print enemies + BACK
        for (int i = 0; i < static_cast<int>(enemies.size()); i++) {
            cout << (i + 1) << ". " << enemies[i]->get_name()
                 << "[HP:" << enemies[i]->get_health() << "] ";
        }
        cout << (enemies.size() + 1) << ". BACK\n";

        input = read_int_choice();

        // BACK
        if (input == static_cast<int>(enemies.size()) + 1) {
            PlayerMenu(npc);
            return true; // go back to PlayerTurn menu
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
        target->take_damage(total_damage);
        cout << "*-------------------------------------------------------*\n";
        cout << "You hit " << target->get_name() << "!\n";
        cout << "It took " << total_damage << " damage!\n";

        if (!target->is_alive()) {
            cout << target->get_name() << " is defeated!\n";
        }
        return false;

    } while (loop);

    return false;
}

// ------------------------------------------------------------
// Basic Implementation of player magic function
bool Battle::BasicPlayerMagic(Character* npc) {
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

        input = read_int_choice();

        // BACK
        if (input == static_cast<int>(enemies.size()) + 1) {
            return true;
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
        if (npc->get_mana() < 10) {
            cout << "*-------------------------------------------------------*\n";
            cout << "Not enough mana!\n";
            loop = true;
           continue;
        }
        else {

            target->take_damage(30);
            npc->spend_mana(10);

            cout << "*-------------------------------------------------------*\n";
            cout << "You hit " << target->get_name() << " with a fireball!\n";
            target->status_handler(StatusCondition::Burn, 5);
            cout << "It took 30 damage!\n";

            if (!target->is_alive()) {
                cout << target->get_name() << " is defeated!\n";
            }
            return false;
        }
    } while (loop);

    return false;
}

// ------------------------------------------------------------

Battle::Battle(vector<Character*> good, vector<Character*> evil, Inventory* inventory)
    : Scene("Battle")
{
    heroes = good;
    enemies = evil;
    partyInventory = inventory;
    initialEnemyCount = static_cast<int>(evil.size());

    DecideTurnOrder();
    Setup();
}

void Battle::Setup() {
    if (turnCounter == -1) {
        cout << "Stand guard, enemies appeared!\n";
        PlaySoundCue(SoundCue::Attack);
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
        PlaySoundCue(SoundCue::End);
        AwardVictoryLoot();
    } else if (result == 2) {
        cout << "You lose!\n";
        PlaySoundCue(SoundCue::Error);
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
    //recover a little mana!
    npc->restore_mana(5);

    cout << dividerFlourish << std::endl;
    cout << "It is " << npc->get_name() << "'s turn!\n";
    npc->status_handler(StatusCondition::None, 0);
    cout << "Health: " << npc->get_health() << "/" << npc-> get_max_health()
         << " Mana: " << npc->get_mana() << "/" << npc->get_max_mana()
         << " Attack Bonus: " << npc->get_attack() << "\n";

    PlayerMenu(npc);

}

void Battle::PlayerMenu(Character* npc) {
    int input = 0;

    std::cout << dividerFlourish << std::endl;
    cout << R"(Choose an action!
1. ATTACK    2. MAGIC    3. ITEMS    4. ESCAPE
)" << endl;

    PlaySoundCue(SoundCue::Menu);
    input = read_int_choice();

    switch (input) {
    case 1:
        //BasicPlayerAttack(npc);
        PlayerAttack(npc, Physical);
        break;
    case 2:
        //BasicPlayerMagic(npc);
        PlayerAttack(npc, Magic);
        break;
    case 3:
        AccessInventory();
        PlayerMenu(npc);
        break;
    case 4:
        std::cout << dividerFlourish << std::endl;
        cout << R"(
You tried to run away, but it failed!
)" << endl;
        PlaySoundCue(SoundCue::Error);
        PlayerMenu(npc);
        break;

    default:
        PlaySoundCue(SoundCue::Error);
        std::cout << "Invalid input! Try again!" << std::endl;
        PlayerMenu(npc);
    }
}

void Battle::PlayerAttack(Character* npc, Category type) {
    //diplay attacks and get positions
    std::cout << dividerFlourish << std::endl;
    std::vector<int> options = npc->display_actions(type);
    if (options.empty()) {
        PlaySoundCue(SoundCue::Error);
        std::cout << "No actions of that type are available.\n";
        PlayerMenu(npc);
        return;
    }

    int input = 0;
    input = read_int_choice();
    const int backOption = static_cast<int>(options.size()) + 1;

    //BACK section: returns to playerMenu
    if (input == backOption) {
        PlayerMenu(npc);
        return;

    }//out of range sections: gives error message and runs playerAttack again
    else if (input < 1 || input > backOption) {

        std::cout << dividerFlourish << std::endl;
        PlaySoundCue(SoundCue::Error);
        std::cout << "Invalid input! Please try again." << std::endl;

        PlayerAttack(npc, type);
        return;

    }//run selected attack section
    else {
        const std::array<int, 6>& actionIds = npc->get_action_ids();
        const int actionIndex = options[input - 1];
        if (actionIndex < 0 || actionIndex >= static_cast<int>(actionIds.size())) {
            PlaySoundCue(SoundCue::Error);
            std::cout << "Invalid action selection.\n";
            PlayerMenu(npc);
            return;
        }

        ActionData moveChoice = GetAction(actionIds[actionIndex]);
        if (moveChoice.name == "Unknown Action") {
            PlaySoundCue(SoundCue::Error);
            std::cout << "That action is unavailable.\n";
            PlayerMenu(npc);
            return;
        }

        Character* targetChoice = choose_enemy_target(enemies);
        if (!targetChoice) {
            PlayerMenu(npc);
            return;
        }
        npc->execute_attack(moveChoice, targetChoice);
    }
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
    PlaySoundCue(SoundCue::Attack);
    heroes[idx]->take_damage(dmg);

    cout << "*-------------------------------------------------------*\n";
    //print enemy info
    std::cout << npc->get_name() << "'s turn! Health: " << npc->get_health() << "/" << npc->get_max_health() << std::endl;
    npc->status_handler(StatusCondition::None, 0);
    cout << npc->get_name() << " wacked " << heroes[idx]->get_name() << "!\n";
    heroes[idx]->status_handler(StatusCondition::Poison, 4);
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

void Battle::AwardVictoryLoot() {
    if (!partyInventory) {
        return;
    }

    Character* rewardRecipient = nullptr;
    for (Character* hero : heroes) {
        if (hero) {
            rewardRecipient = hero;
            break;
        }
    }

    if (!rewardRecipient) {
        return;
    }

    const LootDropResult reward =
        AwardBattleLoot(*rewardRecipient, *partyInventory, initialEnemyCount);
    cout << reward.message << "\n";
}

void Battle::AccessInventory() {
    cout << "*-------------------------------------------------------*\n";
    PlaySoundCue(SoundCue::Error);
    cout << "Inventory access is disabled in battle.\n";
    cout << "Use the Manage Inventory menu outside of combat.\n";
}

void Battle::MenuOptions() {
    return;
}
