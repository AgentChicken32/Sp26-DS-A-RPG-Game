#pragma once

#include "inventory/Inventory.h"
#include "world/WorldMap.h"
#include "characters/character.h"

bool TravelToNewRegion(AdventureState& adventure);
bool ExploreRegion(AdventureState& adventure,
                   Character& hero,
                   Inventory& inventory);
bool ProcessStoryEvent(AdventureState& adventure,
                       Character& hero,
                       Inventory& inventory);
