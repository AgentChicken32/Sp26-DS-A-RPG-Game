#include "character.h"

Character::Character(std::string name, Stats stats)
    : m_name(std::move(name)), m_stats(stats) {
    clamp_all();
}

const std::string& Character::get_name() const { return m_name; }
void Character::set_name(const std::string& name) { m_name = name; }

const Character::Stats& Character::get_stats() const { return m_stats; }

int Character::get_health() const { return m_stats.health; }
int Character::get_mana() const { return m_stats.mana; }
int Character::get_stamina() const { return m_stats.stamina; }
int Character::get_gold() const { return m_stats.gold; }
int Character::get_attack() const { return m_stats.attack; }

int Character::get_max_health() const { return m_stats.max_health; }
int Character::get_max_mana() const { return m_stats.max_mana; }
int Character::get_max_stamina() const { return m_stats.max_stamina; }

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

void Character::status_handler(int status, int chanceOf) {
    //variables
    std::random_device rd;
    std::mt19937 gen(rd());

    //check current status, return if the input is same as current status
    if (m_stats.statusCondition == status) { return; }

    //normal->inflicted
    if (m_stats.statusCondition == 0) {
        std::uniform_int_distribution<int> inflict(1, chanceOf);

        if (inflict(gen) == 1) {
            m_stats.statusCondition = status;
            switch (status) {
            case 1:
                std::cout << get_name() << " was poisoned!" << std::endl;
                break;
            case 2:
                std::cout << get_name() << " was set ablaze!" << std::endl;
                break;
            }
        }
    }//inflicted->normal
    else if (m_stats.statusCondition != 0) {
        std::uniform_int_distribution<int> recoverPoison(1, 5);
        std::uniform_int_distribution<int> recoverBurning(1, 3);

        switch (m_stats.statusCondition) {
        case 1://Poison: 1/5 chance to lose effect each turn Deals 2 damage per turn.
            if (recoverPoison(gen) == 1) {
                m_stats.statusCondition = 0;
                std::cout << "The poison lost it's effect!" << std::endl;
            }
            else {
                m_stats.health -= 2;
                std::cout << get_name() << " took 2 poison damage!" << std::endl;
            }

            break;
        case 2://Burning: 1/3 chance to lose effect, 5 damage per tur
            if (recoverBurning(gen) == 1) {
                m_stats.statusCondition = 0;
                std::cout << "The fire went out!" << std::endl;
            }
            else {
                m_stats.health -= 5;
                std::cout << get_name() << " took 5 burning damage!" << std::endl;
            }

            break;
        }
    }
};

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
