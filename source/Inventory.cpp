#include "Inventory.h"

#include <algorithm>

Inventory::~Inventory() {
    clear();
}

void Inventory::add_item(const InventoryItem& item) {
    Node* node = new Node{item, nullptr};
    if (!m_head) {
        m_head = node;
        ++m_size;
        return;
    }

    Node* current = m_head;
    while (current->next) {
        current = current->next;
    }
    current->next = node;
    ++m_size;
}

bool Inventory::try_add_item(const InventoryItem& item) {
    if (is_full()) {
        return false;
    }

    add_item(item);
    return true;
}

bool Inventory::remove_item_by_name(const std::string& name) {
    Node* previous = nullptr;
    Node* current = m_head;

    while (current) {
        if (current->item.name == name) {
            if (previous) {
                previous->next = current->next;
            } else {
                m_head = current->next;
            }
            delete current;
            if (m_size > 0) {
                --m_size;
            }

            if (name == m_equipped_weapon_name && count_by_name(name) == 0) {
                m_equipped_weapon_name.clear();
            }
            return true;
        }
        previous = current;
        current = current->next;
    }

    return false;
}

bool Inventory::has_item(const std::string& name) const {
    return find_first_by_name(name) != nullptr;
}

int Inventory::count_by_name(const std::string& name) const {
    int count = 0;
    Node* current = m_head;
    while (current) {
        if (current->item.name == name) {
            count++;
        }
        current = current->next;
    }
    return count;
}

bool Inventory::empty() const {
    return m_head == nullptr;
}

bool Inventory::is_full() const {
    return m_size >= capacity();
}

std::size_t Inventory::size() const {
    return m_size;
}

std::size_t Inventory::capacity() const {
    return kCapacity;
}

std::size_t Inventory::remaining_capacity() const {
    if (m_size >= capacity()) {
        return 0;
    }

    return capacity() - m_size;
}

std::vector<ItemSummary> Inventory::summarize() const {
    std::vector<ItemSummary> summary;
    Node* current = m_head;

    while (current) {
        auto it = std::find_if(
            summary.begin(),
            summary.end(),
            [&](const ItemSummary& entry) { return entry.name == current->item.name; });

        if (it == summary.end()) {
            summary.push_back(ItemSummary{
                current->item.name,
                current->item.type,
                1,
                current->item.attack_bonus
            });
        } else {
            it->count++;
        }
        current = current->next;
    }

    return summary;
}

std::vector<InventoryItem> Inventory::dump_items() const {
    std::vector<InventoryItem> items;
    Node* current = m_head;
    while (current) {
        items.push_back(current->item);
        current = current->next;
    }
    return items;
}

void Inventory::load_items(const std::vector<InventoryItem>& items,
                           const std::string& equipped_weapon_name) {
    clear();
    for (const auto& item : items) {
        add_item(item);
    }

    m_equipped_weapon_name.clear();
    if (!equipped_weapon_name.empty()) {
        equip_weapon(equipped_weapon_name);
    }
}

bool Inventory::equip_weapon(const std::string& name) {
    Node* node = find_first_by_name(name);
    if (!node || node->item.type != InventoryItem::Type::Weapon) {
        return false;
    }
    m_equipped_weapon_name = node->item.name;
    return true;
}

void Inventory::unequip_weapon() {
    m_equipped_weapon_name.clear();
}

const InventoryItem* Inventory::equipped_weapon() const {
    if (m_equipped_weapon_name.empty()) {
        return nullptr;
    }

    Node* node = find_first_by_name(m_equipped_weapon_name);
    return node ? &node->item : nullptr;
}

int Inventory::equipped_attack_bonus() const {
    const InventoryItem* weapon = equipped_weapon();
    return weapon ? weapon->attack_bonus : 0;
}

Inventory::Node* Inventory::find_first_by_name(const std::string& name) const {
    Node* current = m_head;
    while (current) {
        if (current->item.name == name) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

void Inventory::clear() {
    Node* current = m_head;
    while (current) {
        Node* next = current->next;
        delete current;
        current = next;
    }
    m_head = nullptr;
    m_equipped_weapon_name.clear();
    m_size = 0;
}
