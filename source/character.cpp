#include "character.h"

// character constructor
character::character(int health, int mana, int stamina, int gold)
    : health(health), mana(mana), stamina(stamina), gold(gold) {}

// virtual destructor
character::~character() {}

// getters
int character::get_health() const {
    return health;
}

int character::get_mana() const {
    return mana;
}

int character::get_stamina() const {
    return stamina;
}

int character::get_gold() const {
    return gold;
}

// setters
void character::set_health(int value) {
    health = value;
}

void character::set_mana(int value) {
    mana = value;
}

void character::set_stamina(int value) {
    stamina = value;
}

void character::set_gold(int value) {
    gold = value;
}

// player
player::player()
    : character(120, 80, 70, 100) {}

// enemy
enemy::enemy(int level)
    : character(50 + level * 10, 20, 40, 10) {}

// npc
npc::npc()
    : character(100, 0, 50, 25) {}
