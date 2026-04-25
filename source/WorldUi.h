#pragma once

#include "Inventory.h"
#include "WorldMap.h"
#include "character.h"

void PrintWorldMapAndJournal(const AdventureState& adventure,
                             const Character& hero,
                             const Inventory& inventory);
void VisitShop(AdventureState& adventure,
               Character& hero,
               Inventory& inventory);
void PrintMainMenu(const AdventureState& adventure,
                   const Character& hero,
                   const Inventory& inventory);
