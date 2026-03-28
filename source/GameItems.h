#pragma once

#include <string>
#include <vector>

#include "Inventory.h"

namespace GameItems {

const std::vector<InventoryItem>& Catalog();
const InventoryItem* FindByName(const std::string& name);

} // namespace GameItems
