#pragma once

#include "inventory/Inventory.h"
#include "world/WorldMap.h"
#include "characters/character.h"

void PrintWorldMapAndJournal(const AdventureState& adventure,
                             const Character& hero,
                             const Inventory& inventory);
void VisitShop(AdventureState& adventure,
               Character& hero,
               Inventory& inventory);
void PrintMainMenu(const AdventureState& adventure,
                   const Character& hero,
                   const Inventory& inventory);
