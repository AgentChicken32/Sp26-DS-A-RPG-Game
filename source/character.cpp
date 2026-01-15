#include <iostream>

class character {
protected:
    int health;
    int mana;
    int stamina;
    int gold;

public:
    // constructor
    character(int health = 100, int mana = 50, int stamina = 50, int gold = 0)
        : health(health), mana(mana), stamina(stamina), gold(gold) {}

    // virtual destructor
    virtual ~character() {}

    // getters
    int get_health() const { return health; }
    int get_mana() const { return mana; }
    int get_stamina() const { return stamina; }
    int get_gold() const { return gold; }

    // setters
    void set_health(int value) { health = value; }
    void set_mana(int value) { mana = value; }
    void set_stamina(int value) { stamina = value; }
    void set_gold(int value) { gold = value; }
};

// example derived classes
class player : public character {
public:
    player() : character(120, 80, 70, 100) {}
};

class enemy : public character {
public:
    enemy(int level) : character(50 + level * 10, 20, 40, 10) {}
};

class npc : public character {
public:
    npc() : character(100, 0, 50, 25) {}
};

// test driver
int main() {
    player p;
    enemy e(3);
    npc n;

    std::cout << "player health: " << p.get_health() << '\n';
    std::cout << "enemy health: " << e.get_health() << '\n';
    std::cout << "npc gold: " << n.get_gold() << '\n';

    return 0;
}
