#include "GameItems.h"

#include <algorithm>

namespace GameItems {

const std::vector<InventoryItem>& Catalog() {
    static const std::vector<InventoryItem> kCatalog = {
        {"Wooden Sword", InventoryItem::Type::Weapon, 5, 10,
         "A simple training sword."},
        {"Iron Sword", InventoryItem::Type::Weapon, 10, 25,
         "Reliable steel blade."},
        {"Training Dagger", InventoryItem::Type::Weapon, 3, 8,
         "Light and quick."},
        {"Potion", InventoryItem::Type::Consumable, 0, 5,
         "Restores a little health."},
        {"Herb", InventoryItem::Type::Misc, 0, 1,
         "Common ingredient."}
    };

    return kCatalog;
}

const InventoryItem* FindByName(const std::string& name) {
    const auto& catalog = Catalog();
    const auto it = std::find_if(
        catalog.begin(),
        catalog.end(),
        [&](const InventoryItem& item) { return item.name == name; });

    if (it == catalog.end()) {
        return nullptr;
    }

    return &(*it);
}

} // namespace GameItems
