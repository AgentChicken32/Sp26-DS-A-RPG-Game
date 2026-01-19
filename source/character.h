#pragma once

class character {
protected:
    int health;
    int mana;
    int stamina;
    int gold;

public:
    // constructor
    character(int health = 100, int mana = 50, int stamina = 50, int gold = 0);

    // virtual destructor
    virtual ~character();

    // getters
    int get_health() const;
    int get_mana() const;
    int get_stamina() const;
    int get_gold() const;

    // setters
    void set_health(int value);
    void set_mana(int value);
    void set_stamina(int value);
    void set_gold(int value);
};

// derived classes
class player : public character {
public:
    player();
};

class enemy : public character {
public:
    enemy(int level);
};

class npc : public character {
public:
    npc();
};