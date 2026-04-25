#pragma once

#include "Inventory.h"
#include "WorldMap.h"
#include "character.h"

bool TravelToNewRegion(AdventureState& adventure);
bool ExploreRegion(AdventureState& adventure,
                   Character& hero,
                   Inventory& inventory);
bool ProcessStoryEvent(AdventureState& adventure,
                       Character& hero,
                       Inventory& inventory);
