#include "characters/character.h"
#include "platform/Sound.h"

Character::Character(std::string name, Stats stats)
    : m_name(std::move(name)), m_stats(stats) {
    clamp_all();
}

namespace {

const char* StatusName(StatusCondition status) {
    switch (status) {
    case StatusCondition::None:
        return "nothing";
    case StatusCondition::Poison:
        return "poison";
    case StatusCondition::Burn:
        return "burn";
    case StatusCondition::Frozen:
        return "freezing";
    }
    return "nothing";
}

} // namespace

const std::string& Character::get_name() const { return m_name; }
void Character::set_name(const std::string& name) { m_name = name; }

const Character::Stats& Character::get_stats() const { return m_stats; }

int Character::get_health() const { return m_stats.health; }
int Character::get_mana() const { return m_stats.mana; }
int Character::get_stamina() const { return m_stats.stamina; }
int Character::get_gold() const { return m_stats.gold; }
int Character::get_attack() const { return m_stats.attack; }
StatusCondition Character::get_status_condition() const { return m_stats.statusCondition; }

int Character::get_max_health() const { return m_stats.max_health; }
int Character::get_max_mana() const { return m_stats.max_mana; }
int Character::get_max_stamina() const { return m_stats.max_stamina; }

const std::array<int, 6>& Character::get_action_ids() const { return m_stats.actions; }

bool Character::is_alive() const { return m_stats.health > 0; }

void Character::take_damage(int amount) {
    if (amount <= 0) return;
    m_stats.health = std::max(0, m_stats.health - amount);
}

void Character::heal(int amount) {
    if (amount <= 0) return;
    m_stats.health = std::min(m_stats.max_health, m_stats.health + amount);
}

bool Character::spend_mana(int amount) {
    if (amount <= 0) return true;
    if (m_stats.mana < amount) return false;
    m_stats.mana -= amount;
    return true;
}

void Character::restore_mana(int amount) {
    if (amount <= 0) return;
    m_stats.mana = std::min(m_stats.max_mana, m_stats.mana + amount);
}

bool Character::spend_stamina(int amount) {
    if (amount <= 0) return true;
    if (m_stats.stamina < amount) return false;
    m_stats.stamina -= amount;
    return true;
}

void Character::restore_stamina(int amount) {
    if (amount <= 0) return;
    m_stats.stamina = std::min(m_stats.max_stamina, m_stats.stamina + amount);
}

bool Character::spend_gold(int amount) {
    if (amount <= 0) return true;
    if (m_stats.gold < amount) return false;
    m_stats.gold -= amount;
    return true;
}

void Character::add_gold(int amount) {
    if (amount <= 0) return;
    m_stats.gold += amount;
}

void Character::set_attack(int value) {
    m_stats.attack = std::max(0, value);
}

bool Character::try_inflict_status(StatusCondition status, double chance) {
    if (status == StatusCondition::None ||
        m_stats.statusCondition != StatusCondition::None) {
        return false;
    }

    chance = std::clamp(chance, 0.0, 1.0);
    if (chance <= 0.0) {
        return false;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> roll(0.0, 1.0);

    if (roll(gen) > chance) {
        return false;
    }

    m_stats.statusCondition = status;
    switch (status) {
    case StatusCondition::None:
        break;
    case StatusCondition::Poison:
        std::cout << get_name() << " was poisoned!" << std::endl;
        break;
    case StatusCondition::Burn:
        std::cout << get_name() << " was set ablaze!" << std::endl;
        break;
    case StatusCondition::Frozen:
        std::cout << get_name() << " was frozen solid!" << std::endl;
        break;
    }

    return true;
}

void Character::status_handler(StatusCondition status, int chanceOf) {
    //variables
    std::random_device rd;
    std::mt19937 gen(rd());

    //check current status, return if the input is same as current status
    if (m_stats.statusCondition == status) { return; }

    //normal->inflicted
    if (m_stats.statusCondition == StatusCondition::None) {
        if (status == StatusCondition::None || chanceOf <= 0) {
            return;
        }

        std::uniform_int_distribution<int> inflict(1, chanceOf);

        if (inflict(gen) == 1) {
            m_stats.statusCondition = status;
            switch (status) {
            case StatusCondition::None:
                break;
            case StatusCondition::Poison:
                std::cout << get_name() << " was poisoned!" << std::endl;
                break;
            case StatusCondition::Burn:
                std::cout << get_name() << " was set ablaze!" << std::endl;
                break;
            case StatusCondition::Frozen:
                std::cout << get_name() << " was frozen solid!" << std::endl;
                break;
            }
        }
    }//inflicted->normal
    else if (m_stats.statusCondition != StatusCondition::None) {
        std::uniform_int_distribution<int> recoverPoison(1, 5);
        std::uniform_int_distribution<int> recoverBurning(1, 3);
        std::uniform_int_distribution<int> recoverFrozen(1, 3);

        switch (m_stats.statusCondition) {
        case StatusCondition::None:
            break;
        case StatusCondition::Poison://Poison: 1/5 chance to lose effect each turn Deals 2 damage per turn.
            if (recoverPoison(gen) == 1) {
                m_stats.statusCondition = StatusCondition::None;
                std::cout << "The poison lost it's effect!" << std::endl;
            }
            else {
                take_damage(2);
                std::cout << get_name() << " took 2 poison damage!" << std::endl;
            }

            break;
        case StatusCondition::Burn://Burning: 1/3 chance to lose effect, 5 damage per tur
            if (recoverBurning(gen) == 1) {
                m_stats.statusCondition = StatusCondition::None;
                std::cout << "The fire went out!" << std::endl;
            }
            else {
                take_damage(5);
                std::cout << get_name() << " took 5 burning damage!" << std::endl;
            }

            break;
        case StatusCondition::Frozen:
            if (recoverFrozen(gen) == 1) {
                m_stats.statusCondition = StatusCondition::None;
                std::cout << get_name() << " thawed out!" << std::endl;
            }
            else {
                std::cout << get_name() << " can't move! "
                          << get_name() << " is frozen solid!" << std::endl;
            }

            break;
        }
    }
};

void Character::execute_attack(ActionData action, Character* target) {
    std::string dividerFlourish = "*-------------------------------------------------------*";
    std::cout << dividerFlourish << std::endl;

    if (target == nullptr) {
        PlaySoundCue(SoundCue::Error);
        std::cout << "No valid target selected." << std::endl;
        return;
    }

    ////////////////
    //MANA SECTION//
    ////////////////
    //subtract mana from total
    if (action.manaCost <= get_mana() && action.category == Magic) {
        spend_mana(action.manaCost);
        PlaySoundCue(SoundCue::Magic);
        std::cout << get_name() << " cast " << action.name << " using " << action.manaCost << " points of mana." << std::endl;
    }
    else if (action.category == Magic) {//fail spell if not enough mana
        PlaySoundCue(SoundCue::Error);
        std::cout << get_name() << " cast " << action.name
                  << ", but the spell fizzled out!" << std::endl;
        return;
    }
    else {
        PlaySoundCue(SoundCue::Attack);
    }

    //////////////////////////
    //ACCURACY CHECK SECTION//
    //////////////////////////
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> accuracyCheck(0, 100);

    //check if action will hit
    if (accuracyCheck(gen) > action.accuracy) {
        PlaySoundCue(SoundCue::Error);
        std::cout << get_name() << " used " << action.name
                  << ", but it missed!" << std::endl;
        return;
    }

    ///////////////////
    //EFFECTS SECTION//
    ///////////////////

    //iterate through vector of effects
    for (const auto& effect : action.effects) {

        //check type of effect
        switch (effect.type) {
        case Damage:
        {
            const int attack_bonus = (action.category == Physical) ? get_attack() : 0;
            const int total_damage = effect.power + attack_bonus;
            target->take_damage(total_damage);
            std::cout << get_name() << " hit " << target->get_name()
                      << " with " << action.name << " dealing "
                      << total_damage << " damage!" << std::endl;
            break;
        }
        case Status:
            if (effect.status != StatusCondition::None &&
                !target->try_inflict_status(effect.status, effect.afflictionChance)) {
                std::cout << target->get_name() << " resisted "
                          << StatusName(effect.status) << "."
                          << std::endl;
            }
            break;
        case Buff:
            //not implemented yet
            break;
        case Debuff:
            //nor is this
            break;
        case Heal:
            heal(effect.power);
            std::cout << get_name() << " restored " << effect.power
                      << " points of health! Health: "
                      << get_health() << "/" << get_max_health()
                      << std::endl;
            break;
        }
    }
}

std::vector<int> Character::display_actions(Category type) {
    int counter = 1;//for display
    int posCounter = 0;//save positions in id array
    std::vector<int> idPositions;

    //take input type and iterate through list of action ids, find data, and display names
    for (int id : get_action_ids()) {
        const auto actionIt = actionDatabase.find(id);
        if (actionIt == actionDatabase.end()) {
            posCounter++;
            continue;
        }

        const ActionData& action = actionIt->second;

        if (action.category == type) {

            std::cout << counter << ". " << action.name;

            //display mana cost: ugly for the time being
            if (type == Magic) {
                std::cout << "[" << action.manaCost << "]";
            }

            //spacing
            std::cout << "  ";

            //make new line every 2 actions
            if ((counter % 2) == 0) {
                std::cout << std::endl;
            }

            counter++;

            //save positions in id array
            idPositions.push_back(posCounter);
        }

        posCounter++;
    }

    //display back
    std::cout << counter << ". BACK" << std::endl;

    //returning to make player decisions possible
    return idPositions;
}

void Character::clamp_all() {
    m_stats.max_health  = std::max(0, m_stats.max_health);
    m_stats.max_mana    = std::max(0, m_stats.max_mana);
    m_stats.max_stamina = std::max(0, m_stats.max_stamina);

    m_stats.health =
        std::max(0, std::min(m_stats.health, m_stats.max_health));
    m_stats.mana =
        std::max(0, std::min(m_stats.mana, m_stats.max_mana));
    m_stats.stamina =
        std::max(0, std::min(m_stats.stamina, m_stats.max_stamina));

    m_stats.attack = std::max(0, m_stats.attack);
    m_stats.gold = std::max(0, m_stats.gold);
}

// --------------------- Player ---------------------

Player::Player(const std::string& name)
    : Character(
          name,
          Character::Stats{
              .max_health = 125,
              .health = 125,
              .max_mana = 50,
              .mana = 50,
              .max_stamina = 80,
              .stamina = 80,
              .gold = 25
          }) {}

// --------------------- Enemy ---------------------

Enemy::Enemy(int level, const std::string& name)
    : Character(
          name,
          Character::Stats{
              .max_health = std::max(1, 50 + (level * 20)),
              .health = std::max(1, 50 + (level * 20)),
              .max_mana = 0,
              .mana = 0,
              .max_stamina = 30,
              .stamina = 30,
              .gold = 0
          }),
      m_level(std::max(1, level)) {}

int Enemy::get_level() const {return m_level;}

void Enemy::enemy_ai([[maybe_unused]] std::vector<Character*> heroes,
                     [[maybe_unused]] std::vector<Character*> enemies) {

    //can I kill anyone?

    // Am I about to die?

    //choose target

}

// --------------------- NPC ---------------------

NPC::NPC(const std::string& name)
    : Character(
        name,
        Character::Stats{
              .max_health = 100,
              .health = 100,
              .max_mana = 0,
              .mana = 0,
              .max_stamina = 0,
              .stamina = 0,
              .gold = 0
          }) {}
