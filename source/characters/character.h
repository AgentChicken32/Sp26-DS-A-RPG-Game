#pragma once

#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <vector>
#include <array>
#include "data/ActionObject.h"

class Character {
public:

    //Members
    struct Stats {
        int max_health   = 0;
        int health       = 0;
        int max_mana     = 0;
        int mana         = 0;
        int max_stamina  = 0;
        int stamina      = 0;
        int attack       = 0;
        int gold         = 0;
        std::array<int, 6> actions;
        StatusCondition statusCondition = StatusCondition::None;
    };


    Character(std::string name, Stats stats);
    virtual ~Character() = default;

    // Identity
    const std::string& get_name() const;
    void set_name(const std::string& name);

    // Stats access
    const Stats& get_stats() const;

    int  get_health() const;
    int  get_mana() const;
    int  get_stamina() const;
    int  get_gold() const;
    int& get_gold_ref() { return m_stats.gold; }
    int  get_attack() const;
    StatusCondition get_status_condition() const;

    int  get_max_health() const;
    int  get_max_mana() const;
    int  get_max_stamina() const;

    const std::array<int, 6>& get_action_ids() const;

    bool is_alive() const;

    // Mutators (safe, clamped)
    void take_damage(int amount);
    void heal(int amount);

    bool spend_mana(int amount);
    void restore_mana(int amount);

    bool spend_stamina(int amount);
    void restore_stamina(int amount);

    bool spend_gold(int amount);
    void add_gold(int amount);

    void set_attack(int value);
    bool try_inflict_status(StatusCondition status, double chance);

    //How to use: status 0 is normal, 1 is poisoned, 2 is burning etc
    //chanceOf decides what the chance of contracting the status is other than normal, so an input of 1 is 100%, 5 is 20%, 25 is 4%, etc.
    void status_handler(StatusCondition status, int chanceOf);

    //runs attack action using chosen action against chosen target
    void execute_attack(ActionData action, Character* target);

    //Displays Actions Character has: Argument is of type Category an enum located in ActionObject.h
    std::vector<int> display_actions(Category type);

protected:
    void clamp_all();

    std::string m_name;
    Stats m_stats;
};

// --------------------- Player ---------------------

class Player : public Character {
public:
    explicit Player(const std::string& name);

    

};

// --------------------- Enemy ---------------------

class Enemy : public Character {
public:
    explicit Enemy(int level, const std::string& name = "Enemy");
    int get_level() const;

    //input vector of hero party and enemy gang, the function checks game state and makes a decision based on that
    void enemy_ai(std::vector<Character*> heroes, std::vector<Character*> enemies);

private:
    int m_level = 1;
};

// --------------------- NPC ---------------------

class NPC : public Character {
public:
    explicit NPC(const std::string& name = "NPC");
};
