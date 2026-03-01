#pragma once

#include <string>
#include <vector>

struct InventoryItem {
    enum class Type { Weapon, Armor, Consumable, Misc };

    std::string name;
    Type type = Type::Misc;
    int attack_bonus = 0;
    int value = 0;
    std::string description;
};

struct ItemSummary {
    std::string name;
    InventoryItem::Type type = InventoryItem::Type::Misc;
    int count = 0;
    int attack_bonus = 0;
};

class Inventory {
public:
    Inventory() = default;
    ~Inventory();

    Inventory(const Inventory&) = delete;
    Inventory& operator=(const Inventory&) = delete;

    void add_item(const InventoryItem& item);
    bool remove_item_by_name(const std::string& name);
    bool has_item(const std::string& name) const;
    int count_by_name(const std::string& name) const;
    bool empty() const;

    std::vector<ItemSummary> summarize() const;
    std::vector<InventoryItem> dump_items() const;
    void load_items(const std::vector<InventoryItem>& items,
                    const std::string& equipped_weapon_name);

    bool equip_weapon(const std::string& name);
    void unequip_weapon();
    const InventoryItem* equipped_weapon() const;
    int equipped_attack_bonus() const;

private:
    struct Node {
        InventoryItem item;
        Node* next = nullptr;
    };

    Node* m_head = nullptr;
    std::string m_equipped_weapon_name;

    Node* find_first_by_name(const std::string& name) const;
    void clear();
};
