#include "battle/BattleClass.h"

#include "battle/LootTable.h"
#include "characters/character.h"
#include "inventory/InventoryMenu.h"
#include "platform/Sound.h"
#include "platform/UiCommon.h"

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

namespace {

vector<Character*> LivingTargets(const vector<Character*>& group)
{
    vector<Character*> alive;
    for (Character* character : group) {
        if (character && character->is_alive()) {
            alive.push_back(character);
        }
    }
    return alive;
}

bool ActionTargetsEnemy(const ActionData& action)
{
    for (const auto& effect : action.effects) {
        switch (effect.type) {
        case EffectType::Damage:
        case EffectType::Status:
        case EffectType::Buff:
        case EffectType::Debuff:
            return true;
        case EffectType::Heal:
            break;
        }
    }
    return false;
}

int HealingPower(const ActionData& action)
{
    int healing = 0;
    for (const auto& effect : action.effects) {
        if (effect.type == EffectType::Heal) {
            healing += effect.power;
        }
    }
    return healing;
}

Character* ChooseWeakestTarget(const vector<Character*>& targets)
{
    vector<Character*> alive = LivingTargets(targets);
    if (alive.empty()) {
        return nullptr;
    }

    return *min_element(
        alive.begin(),
        alive.end(),
        [](Character* lhs, Character* rhs) {
            return lhs->get_health() < rhs->get_health();
        });
}

} // namespace

static Character* choose_enemy_target(vector<Character*>& enemies) {
    vector<Character*> alive = LivingTargets(enemies);

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

        const int input = ReadIntChoice();
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

        input = ReadIntChoice();

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

        input = ReadIntChoice();

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
    bool escaped = false;

    while ((result = CheckForWinLoss()) == 0 && !escaped) {
        if (turnOrder.empty()) {
            break;
        }

        Character* actor = turnOrder[turnCounter];
        bool is_enemy =
            (std::find(enemies.begin(), enemies.end(), actor) != enemies.end());

        if (is_enemy) {
            SmartEnemyTurn(actor);
        } else {
            const bool attempted_escape = PlayerTurn(actor);
            if (attempted_escape) {
                escaped = RunAway();
                if (!escaped) {
                    cout << "But you couldn't escape!" << endl;
                    PlaySoundCue(SoundCue::Error);
                }
            }
        }

        if (!escaped && !turnOrder.empty()) {
            turnCounter++;
            if (turnCounter >= static_cast<int>(turnOrder.size())) {
                turnCounter = 0;
            }
        }
    }

    if (escaped) {
        cout << "You escaped!\n";
        PlaySoundCue(SoundCue::End);
    } else if (result == 1) {
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

bool Battle::PlayerTurn(Character* npc) {
    npc->restore_mana(5);

    cout << dividerFlourish << std::endl;
    cout << "It is " << npc->get_name() << "'s turn!\n";
    npc->status_handler(StatusCondition::None, 0);
    if (!npc->is_alive()) {
        return false;
    }

    cout << "Health: " << npc->get_health() << "/" << npc->get_max_health()
         << " Mana: " << npc->get_mana() << "/" << npc->get_max_mana()
         << " Attack Bonus: " << npc->get_attack() << "\n";

    if (npc->get_status_condition() == StatusCondition::Frozen) {
        WaitForEnter();
        return false;
    }

    return PlayerMenu(npc);
}

bool Battle::PlayerMenu(Character* npc) {
    while (true) {
        std::cout << dividerFlourish << std::endl;
        cout << R"(Choose an action!
1. ATTACK    2. MAGIC    3. ITEMS    4. ESCAPE
)" << endl;

        PlaySoundCue(SoundCue::Menu);
        const int input = ReadIntChoice();

        switch (input) {
        case 1:
            if (PlayerAttack(npc, Physical)) {
                return false;
            }
            break;
        case 2:
            if (PlayerAttack(npc, Magic)) {
                return false;
            }
            break;
        case 3:
            AccessInventory();
            break;
        case 4:
            std::cout << dividerFlourish << std::endl;
            cout << "You tried to run away!" << endl;
            return true;
        default:
            PlaySoundCue(SoundCue::Error);
            std::cout << "Invalid input! Try again!" << std::endl;
            break;
        }
    }
}

bool Battle::PlayerAttack(Character* npc, Category type) {
    while (true) {
        std::cout << dividerFlourish << std::endl;
        std::vector<int> options = npc->display_actions(type);
        if (options.empty()) {
            PlaySoundCue(SoundCue::Error);
            std::cout << "No actions of that type are available.\n";
            return false;
        }

        const int input = ReadIntChoice();
        const int backOption = static_cast<int>(options.size()) + 1;

        if (input == backOption) {
            return false;
        }

        if (input < 1 || input > backOption) {
            std::cout << dividerFlourish << std::endl;
            PlaySoundCue(SoundCue::Error);
            std::cout << "Invalid input! Please try again." << std::endl;
            continue;
        }

        const std::array<int, 6>& actionIds = npc->get_action_ids();
        const int actionIndex = options[input - 1];
        if (actionIndex < 0 || actionIndex >= static_cast<int>(actionIds.size())) {
            PlaySoundCue(SoundCue::Error);
            std::cout << "Invalid action selection.\n";
            continue;
        }

        ActionData moveChoice = GetAction(actionIds[actionIndex]);
        if (moveChoice.name == "Unknown Action") {
            PlaySoundCue(SoundCue::Error);
            std::cout << "That action is unavailable.\n";
            continue;
        }

        if (!ActionTargetsEnemy(moveChoice)) {
            npc->execute_attack(moveChoice, npc);
            return true;
        }

        Character* targetChoice = choose_enemy_target(enemies);
        if (!targetChoice) {
            continue;
        }

        npc->execute_attack(moveChoice, targetChoice);
        return true;
    }
}

bool Battle::RunAway() {
    if (heroes.empty()) {
        return false;
    }

    vector<Character*> aliveEnemies = LivingTargets(enemies);
    if (aliveEnemies.size() != 1) {
        return false;
    }

    Character* hero = heroes.front();
    Character* enemy = aliveEnemies.front();

    std::random_device rd;
    std::mt19937 gen(rd());

    int difference = (enemy->get_health() - hero->get_health()) / 4;
    if (difference <= 0) {
        return true;
    }

    std::uniform_int_distribution<int> escapeChance(1, difference);
    return escapeChance(gen) == 1;
}

void Battle::EnemyTurn(Character* npc) {
    if (heroes.empty()) return;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> chooseTarget(
        0,
        static_cast<int>(heroes.size()) - 1);
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
    std::cout << npc->get_name() << "'s turn! Health: "
              << npc->get_health() << "/" << npc->get_max_health()
              << std::endl;
    npc->status_handler(StatusCondition::None, 0);
    cout << npc->get_name() << " wacked " << heroes[idx]->get_name() << "!\n";
    heroes[idx]->status_handler(StatusCondition::Poison, 4);
    cout << heroes[idx]->get_name() << " has " << heroes[idx]->get_health()
         << " health left!\n";
}

void Battle::SmartEnemyTurn(Character* npc) {
    vector<Character*> aliveHeroes = LivingTargets(heroes);
    if (aliveHeroes.empty()) {
        return;
    }

    npc->restore_mana(5);

    cout << dividerFlourish << std::endl;
    cout << "It is " << npc->get_name() << "'s turn!\n";
    npc->status_handler(StatusCondition::None, 0);
    if (!npc->is_alive()) {
        return;
    }

    cout << "Health: " << npc->get_health() << "/" << npc->get_max_health()
         << " Mana: " << npc->get_mana() << "/" << npc->get_max_mana()
         << " Attack Bonus: " << npc->get_attack() << "\n";

    if (npc->get_status_condition() == StatusCondition::Frozen) {
        return;
    }

    vector<ActionData> usableActions;
    for (int id : npc->get_action_ids()) {
        if (id == 0) {
            continue;
        }

        ActionData action = GetAction(id);
        if (action.name == "Unknown Action") {
            continue;
        }
        if (action.category == Magic && action.manaCost > npc->get_mana()) {
            continue;
        }

        usableActions.push_back(action);
    }

    if (usableActions.empty()) {
        EnemyTurn(npc);
        return;
    }

    Character* weakestHero = ChooseWeakestTarget(aliveHeroes);
    if (!weakestHero) {
        return;
    }

    ActionData* bestHeal = nullptr;
    for (auto& action : usableActions) {
        if (HealingPower(action) > 0) {
            if (!bestHeal || HealingPower(action) > HealingPower(*bestHeal)) {
                bestHeal = &action;
            }
        }
    }

    if (bestHeal && npc->get_health() < (npc->get_max_health() / 3)) {
        npc->execute_attack(*bestHeal, npc);
        return;
    }

    ActionData* lethalAction = nullptr;
    for (auto& action : usableActions) {
        if (!ActionTargetsEnemy(action)) {
            continue;
        }

        if (GetDamage(action) >= weakestHero->get_health()) {
            if (!lethalAction ||
                action.accuracy > lethalAction->accuracy ||
                (action.accuracy == lethalAction->accuracy &&
                 GetDamage(action) > GetDamage(*lethalAction))) {
                lethalAction = &action;
            }
        }
    }

    if (lethalAction) {
        npc->execute_attack(*lethalAction, weakestHero);
        return;
    }

    ActionData* chosenAction = nullptr;
    double chosenStatusChance = -1.0;
    int chosenDamage = -1;

    for (auto& action : usableActions) {
        if (!ActionTargetsEnemy(action)) {
            continue;
        }

        const int damage = GetDamage(action);
        const double statusChance = CheckIfStatus(action);

        if (!chosenAction) {
            chosenAction = &action;
            chosenDamage = damage;
            chosenStatusChance = statusChance;
            continue;
        }

        if (npc->get_health() > ((npc->get_max_health() / 4) * 3)) {
            if (statusChance > chosenStatusChance ||
                (statusChance == chosenStatusChance && damage > chosenDamage)) {
                chosenAction = &action;
                chosenDamage = damage;
                chosenStatusChance = statusChance;
            }
        } else if (npc->get_health() > (npc->get_max_health() / 2)) {
            if (damage > chosenDamage ||
                (damage == chosenDamage && statusChance > chosenStatusChance)) {
                chosenAction = &action;
                chosenDamage = damage;
                chosenStatusChance = statusChance;
            }
        } else if (npc->get_health() > (npc->get_max_health() / 4)) {
            const int currentScore = chosenAction->accuracy + chosenDamage;
            const int candidateScore = action.accuracy + damage;
            if (candidateScore > currentScore) {
                chosenAction = &action;
                chosenDamage = damage;
                chosenStatusChance = statusChance;
            }
        } else {
            if (action.accuracy > chosenAction->accuracy ||
                (action.accuracy == chosenAction->accuracy && damage > chosenDamage)) {
                chosenAction = &action;
                chosenDamage = damage;
                chosenStatusChance = statusChance;
            }
        }
    }

    if (chosenAction) {
        npc->execute_attack(*chosenAction, weakestHero);
        return;
    }

    EnemyTurn(npc);
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
    if (!partyInventory || heroes.empty() || !heroes[0]) {
        PlaySoundCue(SoundCue::Error);
        cout << "No inventory is available right now.\n";
        return;
    }

    ManageInventory(*partyInventory, *heroes[0]);
}

void Battle::MenuOptions() {
    return;
}
