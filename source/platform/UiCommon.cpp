#include "platform/UiCommon.h"

#include <iostream>
#include <limits>
#include <string>

#include "platform/Sound.h"

int ReadIntChoice()
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

void WaitForEnter()
{
    std::cout << "Press Enter to continue...";
    std::cout.flush();

    std::string ignored;
    std::getline(std::cin, ignored);
}

const char* ItemTypeLabel(InventoryItem::Type type)
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

const char* DangerLabel(int danger_level)
{
    switch (danger_level) {
    case 0:
        return "Safe";
    case 1:
        return "Low";
    case 2:
        return "Guarded";
    case 3:
        return "High";
    default:
        return "Deadly";
    }
}

void PlayErrorSound() { PlaySoundCue(SoundCue::Error); }
void PlayMagicSound() { PlaySoundCue(SoundCue::Magic); }
void PlayMenuSound() { PlaySoundCue(SoundCue::Menu); }
void PlayAttackSound() { PlaySoundCue(SoundCue::Attack); }
void PlayEndSound() { PlaySoundCue(SoundCue::End); }

void FullyRestore(Character& character)
{
    character.heal(character.get_max_health());
    character.restore_mana(character.get_max_mana());
    character.restore_stamina(character.get_max_stamina());
}

void SyncEquippedWeapon(Character& player, const Inventory& inventory)
{
    player.set_attack(inventory.equipped_attack_bonus());
}
