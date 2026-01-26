#pragma once

#include <string>
#include <algorithm>

class Character {
public:
    struct Stats {
        int max_health   = 0;
        int health       = 0;
        int max_mana     = 0;
        int mana         = 0;
        int max_stamina  = 0;
        int stamina      = 0;
        int gold         = 0;
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

    int  get_max_health() const;
    int  get_max_mana() const;
    int  get_max_stamina() const;

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

private:
    int m_level = 1;
};

// --------------------- NPC ---------------------

class NPC : public Character {
public:
    explicit NPC(const std::string& name = "NPC");
};