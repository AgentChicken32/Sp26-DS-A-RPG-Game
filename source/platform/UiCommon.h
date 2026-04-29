#pragma once

#include "inventory/Inventory.h"
#include "characters/character.h"

int ReadIntChoice();
void WaitForEnter();

const char* ItemTypeLabel(InventoryItem::Type type);
const char* DangerLabel(int danger_level);

void PlayErrorSound();
void PlayMagicSound();
void PlayMenuSound();
void PlayAttackSound();
void PlayEndSound();

void FullyRestore(Character& character);
void SyncEquippedWeapon(Character& player, const Inventory& inventory);
