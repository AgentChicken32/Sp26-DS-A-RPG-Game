#include "Inventory.h"
#include "Sound.h"
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

const char* item_type_label(InventoryItem::Type type)
{
    switch (type) {
    case InventoryItem::Type::Weapon:
        return "Weapon";
    case InventoryItem::Type::Armor:
        return "Armor";
    case InventoryItem::Type::Consumable:
        return "Consumable";
    case InventoryItem::Type::Misc:
    default:
        return "Misc";
    }
}

int read_int_choice()
{
    int choice{};
    while (!(std::cin >> choice)) {
        if (std::cin.eof()) {
            return -1;
        }

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Please enter a number: ";
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return choice;
}

void print_inventory_summary(const std::vector<ItemSummary>& summary,
    const InventoryItem* equipped)
{
    std::cout << "\n--- Inventory ---\n";
    for (size_t i = 0; i < summary.size(); ++i) {
        const auto& entry = summary[i];
        std::cout << (i + 1) << ") " << entry.name
            << " x" << entry.count
            << " [" << item_type_label(entry.type) << "]";
        if (entry.type == InventoryItem::Type::Weapon) {
            std::cout << " (+" << entry.attack_bonus << " atk)";
        }
        if (equipped && entry.name == equipped->name) {
            std::cout << " [EQUIPPED]";
        }
        std::cout << "\n";
    }
}

void sync_equipped_weapon(Character& player, const Inventory& inventory)
{
    player.set_attack(inventory.equipped_attack_bonus());
}

void Inventory::manage_inventory(Inventory& inventory, Character& player)
{
    bool in_menu = true;

    while (in_menu) {
        const InventoryItem* equipped = inventory.equipped_weapon();
        const auto summary = inventory.summarize();

        std::cout << "\n=== INVENTORY ===\n";
        std::cout << "Health: " << player.get_health() << "/" << player.get_max_health()
            << " | Mana: " << player.get_mana() << "/" << player.get_max_mana()
            << " | Gold: " << player.get_gold() << "\n";
        std::cout << "Slots: " << inventory.size() << "/" << inventory.capacity()
            << " (" << inventory.remaining_capacity() << " free)\n";
        std::cout << "Equipped weapon: "
            << (equipped ? equipped->name : "None");
        if (equipped) {
            std::cout << " (+" << equipped->attack_bonus << " atk)";
        }
        std::cout << "\n";
        std::cout << "1) View items\n"
            << "2) Use consumable\n"
            << "3) Equip weapon\n"
            << "4) Unequip weapon\n"
            << "5) Discard item\n"
            << "6) Back\n"
            << "Choice: ";

        const int choice = read_int_choice();
        switch (choice) {
        case 1:
            if (summary.empty()) {
                std::cout << "Your inventory is empty.\n";
            }
            else {
                print_inventory_summary(summary, equipped);
            }
            break;
        case 2: {
            std::vector<ItemSummary> consumables;
            for (const auto& item : summary) {
                if (item.type == InventoryItem::Type::Consumable) {
                    consumables.push_back(item);
                }
            }

            if (consumables.empty()) {
                std::cout << "You have no consumables ready.\n";
                break;
            }

            print_inventory_summary(consumables, equipped);
            std::cout << "Select consumable to use (0 to cancel): ";
            const int selection = read_int_choice();
            if (selection == 0) {
                break;
            }
            if (selection < 1 || selection > static_cast<int>(consumables.size())) {
                std::cout << "Invalid choice.\n";
                break;
            }

            const std::string& item_name = consumables[selection - 1].name;
            const int health_before = player.get_health();
            const int mana_before = player.get_mana();

            if (item_name == "Potion") {
                player.heal(35);
                //magicSound();
            }
            else if (item_name == "Herb") {
                player.restore_mana(12);
                //magicSound();
            }
            else {
                std::cout << "You cannot use " << item_name << " yet.\n";
                break;
            }

            inventory.remove_item_by_name(item_name);
            sync_equipped_weapon(player, inventory);

            const int health_restored = player.get_health() - health_before;
            const int mana_restored = player.get_mana() - mana_before;
            std::cout << "You use " << item_name << ". ";
            if (health_restored > 0) {
                std::cout << "Recovered " << health_restored << " health.";
            }
            else if (mana_restored > 0) {
                std::cout << "Recovered " << mana_restored << " mana.";
            }
            else {
                std::cout << "It settles your nerves, even if the numbers do not change.";
            }
            std::cout << "\n";
            break;
        }
        case 3: {
            std::vector<ItemSummary> weapons;
            for (const auto& item : summary) {
                if (item.type == InventoryItem::Type::Weapon) {
                    weapons.push_back(item);
                }
            }

            if (weapons.empty()) {
                std::cout << "No weapons to equip.\n";
                break;
            }

            print_inventory_summary(weapons, equipped);
            std::cout << "Select weapon to equip (0 to cancel): ";
            const int selection = read_int_choice();
            if (selection == 0) {
                break;
            }
            if (selection < 1 || selection > static_cast<int>(weapons.size())) {
                std::cout << "Invalid choice.\n";
                break;
            }

            const std::string& weapon_name = weapons[selection - 1].name;
            if (inventory.equip_weapon(weapon_name)) {
                sync_equipped_weapon(player, inventory);
                std::cout << "Equipped " << weapon_name << ".\n";
                //menuSound();
            }
            else {
                std::cout << "Could not equip " << weapon_name << ".\n";
            }
            break;
        }
        case 4:
            inventory.unequip_weapon();
            sync_equipped_weapon(player, inventory);
            std::cout << "Weapon unequipped.\n";
            break;
        case 5:
            if (summary.empty()) {
                std::cout << "There is nothing to discard.\n";
                break;
            }
            print_inventory_summary(summary, equipped);
            std::cout << "Select item to discard (0 to cancel): ";
            {
                const int selection = read_int_choice();
                if (selection == 0) {
                    break;
                }
                if (selection < 1 || selection > static_cast<int>(summary.size())) {
                    std::cout << "Invalid choice.\n";
                    break;
                }

                const std::string& item_name = summary[selection - 1].name;
                if (inventory.remove_item_by_name(item_name)) {
                    sync_equipped_weapon(player, inventory);
                    std::cout << "Discarded one " << item_name << ".\n";
                }
                else {
                    std::cout << "Could not discard " << item_name << ".\n";
                }
            }
            break;
        case 6:
            in_menu = false;
            break;
        case -1:
            in_menu = false;
            break;
        default:
            //errorSound();
            std::cout << "Invalid choice.\n";
            break;
        }
    }
}