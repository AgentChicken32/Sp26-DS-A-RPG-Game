#include "world/WorldMap.h"

#include <stdexcept>

namespace {

constexpr std::size_t RegionIndex(RegionId id) {
    return static_cast<std::size_t>(id);
}

constexpr std::array<RegionId, kRegionCount> BuildAllRegions() {
    std::array<RegionId, kRegionCount> regions{};
    for (std::size_t i = 0; i < regions.size(); ++i) {
        regions[i] = static_cast<RegionId>(i);
    }
    return regions;
}

constexpr ExplorationItemProfile kDefaultFind{ "Potion", "Herb", 70 };
constexpr ExplorationItemProfile kMountainFind{ "Iron Sword", "Potion", 35 };
constexpr ExplorationItemProfile kPlainsFind{ "Training Dagger", "Herb", 35 };
constexpr ExplorationItemProfile kSouthernFind{ "Herb", "Potion", 50 };

const std::array<RegionId, kRegionCount> kAllRegions = BuildAllRegions();

const std::array<RegionSection, kRegionSectionCount> kRegionSections = {{
    RegionSection::North,
    RegionSection::WesternMainland,
    RegionSection::EasternReach
}};

// Keep these tables in RegionId enum order.
const std::array<RegionData, kRegionCount> kRegions = {{
    {RegionId::IceCourt, "Ice Court",
     "A frozen fortress at the roof of the world, where the wind cuts like glass.",
     4, RegionSection::North,
     {{ "Frostbound Exile", "Rime Wolf" }}, kDefaultFind},
    {RegionId::Gelt, "Gelt",
     "A lonely northern coast of traders and smugglers watching the icepack drift.",
     1, RegionSection::North,
     {{ "Gelt Smuggler", "Harbor Knifehand" }}, kDefaultFind},
    {RegionId::NorthernWilds, "Northern Wilds",
     "Dense pine country full of old trails, hunters, and things that prefer twilight.",
     2, RegionSection::WesternMainland,
     {{ "Wildfang Stalker", "Pinecloak Hunter" }}, kDefaultFind},
    {RegionId::River, "River",
     "The great inland artery where barges, ferries, and hungry reeds crowd the banks.",
     1, RegionSection::WesternMainland,
     {{ "River Drake Whelp", "Reedway Bandit" }}, kDefaultFind},
    {RegionId::Mudlands, "Mudlands",
     "Sodden lowlands where every path shifts beneath your boots.",
     3, RegionSection::WesternMainland,
     {{ "Bog Leech Alpha", "Mudlands Marauder" }}, kDefaultFind},
    {RegionId::Channel, "Channel",
     "A narrow split of water and stone that guides travelers toward the southern roads.",
     1, RegionSection::WesternMainland,
     {{ "Channel Cutthroat", "Bridge Trollkin" }}, kDefaultFind},
    {RegionId::Glade, "Glade",
     "A hush of ancient trees and rune-marked standing stones that remember older bargains.",
     2, RegionSection::WesternMainland,
     {{ "Bramble Warden", "Hollow Deer" }}, kSouthernFind},
    {RegionId::RuneMountains, "Rune Mountains",
     "Jagged peaks carved with glowing script and haunted by hard-bitten raiders.",
     3, RegionSection::WesternMainland,
     {{ "Rune-Touched Raider", "Stonecut Marauder" }}, kMountainFind},
    {RegionId::PatomicCity, "Patomic City",
     "A harbor city of towers, gears, and restless markets built around an aging clockwork ward.",
     0, RegionSection::WesternMainland,
     {{ "Clocktower Thief", "Dockside Ruffian" }}, kDefaultFind},
    {RegionId::CentaurionPlaines, "Centaurion Plaines",
     "Open grassland where the horizon runs forever and hoofbeats carry for miles.",
     1, RegionSection::WesternMainland,
     {{ "Plains Skirmisher", "Hoofstep Ambusher" }}, kPlainsFind},
    {RegionId::SouthernExpanse, "Southern Expanse",
     "A warm and wind-beaten stretch of coast where the road begins to feel forgotten.",
     2, RegionSection::WesternMainland,
     {{ "Dustglass Viper", "Coastline Reaver" }}, kSouthernFind},
    {RegionId::ShrineOfTheWatchmaker, "Shrine of the Watchmaker",
     "An old sea shrine whose mechanisms still whisper when the tides are quiet.",
     0, RegionSection::WesternMainland,
     {{ "Broken Sentinel", "Tide Wraith" }}, kDefaultFind},
    {RegionId::EasternSea, "Eastern Sea",
     "A wide and mercurial sea, beautiful at dawn and dangerous by noon.",
     3, RegionSection::EasternReach,
     {{ "Saltbound Corsair", "Sea-Glass Reaver" }}, kDefaultFind},
    {RegionId::StormSpiralIsles, "Storm / Spiral Isles",
     "A ring of storm-torn isles circling strange waters and stranger lights.",
     4, RegionSection::EasternReach,
     {{ "Spiral Cultist", "Stormwake Harrier" }}, kDefaultFind},
    {RegionId::EasternMountainChain, "Eastern Mountain Chain",
     "Black coastal cliffs and knife-edged passes scoured by sea-born thunder.",
     4, RegionSection::EasternReach,
     {{ "Storm Roc", "Cliffside Ravager" }}, kMountainFind},
    {RegionId::BlinkeringIsle, "Blinkering Isle",
     "A lonely island lighthouse blinking against the pull of spiral currents.",
     3, RegionSection::EasternReach,
     {{ "Blinker Wisp", "Beacon Raider" }}, kDefaultFind},
    {RegionId::Casino, "Casino",
     "The best place in the world!",
     0, RegionSection::WesternMainland,
     {{ "Velvet Bouncer", "Loaded Dice Enforcer" }}, kDefaultFind},
}};
static_assert(kRegions.size() == kRegionCount);

constexpr std::array<ShopStockEntry, kShopSlotCount> ShopStock(
    ShopStockEntry first,
    ShopStockEntry second,
    ShopStockEntry third,
    ShopStockEntry fourth)
{
    return {{ first, second, third, fourth }};
}

// Keep this table in RegionId enum order.
const std::array<RegionShopData, kRegionCount> kRegionShops = {{
    {"Frostgate Provisioner",
     ShopStock({"Potion", 8, 3}, {"Herb", 3, 4},
               {"Wooden Sword", 15, 1}, {"Iron Sword", 40, 1})},
    {"Gelt Dock Exchange",
     ShopStock({"Potion", 6, 4}, {"Herb", 2, 5},
               {"Training Dagger", 11, 1}, {"Wooden Sword", 13, 1})},
    {"Pinewatch Outfitter",
     ShopStock({"Potion", 7, 3}, {"Herb", 2, 6},
               {"Training Dagger", 10, 2}, {"Wooden Sword", 14, 1})},
    {"Riverbend Supply",
     ShopStock({"Potion", 6, 4}, {"Herb", 3, 5},
               {"Training Dagger", 12, 1}, {"Iron Sword", 35, 1})},
    {"Mudlands Pole-and-Pack",
     ShopStock({"Potion", 8, 4}, {"Herb", 4, 4},
               {"Training Dagger", 14, 1}, {"Iron Sword", 38, 1})},
    {"Channel Ferry Stall",
     ShopStock({"Potion", 6, 3}, {"Herb", 3, 4},
               {"Wooden Sword", 13, 1}, {"Training Dagger", 12, 1})},
    {"Glade Root Market",
     ShopStock({"Potion", 7, 3}, {"Herb", 2, 7},
               {"Training Dagger", 11, 1}, {"Iron Sword", 36, 1})},
    {"Rune Mountain Quartermaster",
     ShopStock({"Potion", 9, 3}, {"Herb", 4, 3},
               {"Iron Sword", 32, 2}, {"Training Dagger", 13, 1})},
    {"Patomic Clockmarket",
     ShopStock({"Potion", 5, 5}, {"Herb", 2, 5},
               {"Wooden Sword", 12, 2}, {"Training Dagger", 10, 1})},
    {"Centaurion Roadside Goods",
     ShopStock({"Potion", 6, 4}, {"Herb", 2, 6},
               {"Training Dagger", 9, 2}, {"Wooden Sword", 13, 1})},
    {"Southern Expanse Caravan",
     ShopStock({"Potion", 7, 4}, {"Herb", 3, 5},
               {"Training Dagger", 12, 1}, {"Iron Sword", 34, 1})},
    {"Watchmaker Shrine Almonry",
     ShopStock({"Potion", 5, 3}, {"Herb", 1, 6},
               {"Wooden Sword", 11, 1}, {"Iron Sword", 33, 1})},
    {"Eastern Sea Deck Locker",
     ShopStock({"Potion", 8, 4}, {"Herb", 4, 4},
               {"Training Dagger", 14, 1}, {"Iron Sword", 37, 1})},
    {"Storm-Spiral Salvage",
     ShopStock({"Potion", 10, 4}, {"Herb", 5, 3},
               {"Training Dagger", 15, 1}, {"Iron Sword", 42, 1})},
    {"Eastern Chain Forge",
     ShopStock({"Potion", 9, 3}, {"Herb", 4, 4},
               {"Iron Sword", 30, 2}, {"Training Dagger", 13, 1})},
    {"Blinkering Beacon Stores",
     ShopStock({"Potion", 8, 3}, {"Herb", 3, 5},
               {"Wooden Sword", 14, 1}, {"Iron Sword", 36, 1})},
    {"Casino Cashier Cage",
     ShopStock({"Potion", 9, 3}, {"Herb", 4, 4},
               {"Training Dagger", 16, 1}, {"Iron Sword", 45, 1})}
}};
static_assert(kRegionShops.size() == kRegionCount);

const std::array<std::vector<RegionId>, kRegionCount> kRegionConnections = {{
    {RegionId::Gelt, RegionId::NorthernWilds},
    {RegionId::IceCourt},
    {RegionId::IceCourt, RegionId::River, RegionId::EasternSea},
    {RegionId::NorthernWilds, RegionId::Mudlands, RegionId::Channel},
    {RegionId::River, RegionId::RuneMountains},
    {RegionId::River, RegionId::Glade},
    {RegionId::Channel, RegionId::RuneMountains},
    {RegionId::Glade, RegionId::Mudlands, RegionId::PatomicCity,
     RegionId::EasternSea},
    {RegionId::RuneMountains, RegionId::CentaurionPlaines,
     RegionId::SouthernExpanse, RegionId::EasternSea, RegionId::Casino},
    {RegionId::PatomicCity, RegionId::SouthernExpanse,
     RegionId::EasternSea},
    {RegionId::PatomicCity, RegionId::CentaurionPlaines,
     RegionId::ShrineOfTheWatchmaker, RegionId::EasternSea},
    {RegionId::SouthernExpanse},
    {RegionId::NorthernWilds, RegionId::RuneMountains,
     RegionId::PatomicCity, RegionId::CentaurionPlaines,
     RegionId::SouthernExpanse, RegionId::StormSpiralIsles,
     RegionId::EasternMountainChain, RegionId::BlinkeringIsle},
    {RegionId::EasternSea, RegionId::EasternMountainChain},
    {RegionId::EasternSea, RegionId::StormSpiralIsles,
     RegionId::BlinkeringIsle},
    {RegionId::EasternSea, RegionId::EasternMountainChain},
    {RegionId::PatomicCity}
}};
static_assert(kRegionConnections.size() == kRegionCount);

} // namespace

AdventureState CreateNewAdventure() {
    AdventureState adventure;
    adventure.shop_refresh_stage.fill(-1);
    RefreshAllShopStock(adventure);
    MarkVisited(adventure, adventure.current_region);
    return adventure;
}

const RegionData& GetRegionData(RegionId id) {
    const std::size_t index = RegionIndex(id);
    if (index >= kRegions.size()) {
        throw std::out_of_range("Unknown region id");
    }
    return kRegions[index];
}

const RegionShopData& GetRegionShopData(RegionId id) {
    const std::size_t index = RegionIndex(id);
    if (index >= kRegionShops.size()) {
        throw std::out_of_range("Unknown region shop id");
    }
    return kRegionShops[index];
}

std::vector<RegionId> GetConnectedRegions(RegionId id) {
    const std::size_t index = RegionIndex(id);
    if (index < kRegionConnections.size()) {
        return kRegionConnections[index];
    }
    return {};
}

const std::array<RegionId, kRegionCount>& GetAllRegions() {
    return kAllRegions;
}

const std::array<RegionSection, kRegionSectionCount>& GetRegionSections() {
    return kRegionSections;
}

const char* RegionName(RegionId id) {
    return GetRegionData(id).name;
}

const char* RegionSectionName(RegionSection section) {
    switch (section) {
    case RegionSection::North:
        return "North";
    case RegionSection::WesternMainland:
        return "Western Mainland";
    case RegionSection::EasternReach:
        return "Eastern Reach";
    case RegionSection::Count:
        break;
    }
    return "Unknown";
}

int StoryStageIndex(StoryStage stage) {
    return static_cast<int>(stage);
}

const char* StoryStageName(StoryStage stage) {
    switch (stage) {
    case StoryStage::Prologue:
        return "Prologue";
    case StoryStage::SeekGlade:
        return "Act I: Road To The Glade";
    case StoryStage::SeekShrine:
        return "Act II: The Watchmaker's Road";
    case StoryStage::ReturnToPatomic:
        return "Act III: Storm Over Patomic";
    case StoryStage::DefendPatomic:
        return "Finale: The Spiral Captain";
    case StoryStage::Epilogue:
        return "Epilogue";
    }
    return "Unknown";
}

const char* StoryObjective(const AdventureState& adventure) {
    switch (adventure.story_stage) {
    case StoryStage::Prologue:
        return "Hear what Patomic City's clockkeepers need from you.";
    case StoryStage::SeekGlade:
        return "Reach the Glade and uncover where the old ward road leads.";
    case StoryStage::SeekShrine:
        return "Travel to the Shrine of the Watchmaker in the Southern Expanse.";
    case StoryStage::ReturnToPatomic:
        return "Return to Patomic City before the spiral storm reaches the harbor.";
    case StoryStage::DefendPatomic:
        return "Prepare yourself, then explore Patomic City to confront the Spiral Captain.";
    case StoryStage::Epilogue:
        return "The city is safe for now. Roam the map, explore, and grow stronger.";
    }
    return "Keep moving.";
}

bool TryParseRegionId(const std::string& value, RegionId& out) {
    for (RegionId id : GetAllRegions()) {
        if (value == RegionName(id)) {
            out = id;
            return true;
        }
    }
    return false;
}

bool TryParseStoryStage(const std::string& value, StoryStage& out) {
    const std::array<StoryStage, 6> stages = {{
        StoryStage::Prologue,
        StoryStage::SeekGlade,
        StoryStage::SeekShrine,
        StoryStage::ReturnToPatomic,
        StoryStage::DefendPatomic,
        StoryStage::Epilogue,
    }};

    for (StoryStage stage : stages) {
        if (value == StoryStageName(stage)) {
            out = stage;
            return true;
        }
    }

    return false;
}

bool HasVisited(const AdventureState& adventure, RegionId id) {
    return adventure.visited[RegionIndex(id)];
}

void MarkVisited(AdventureState& adventure, RegionId id) {
    adventure.visited[RegionIndex(id)] = true;
}

void RefreshShopStock(AdventureState& adventure, RegionId id) {
    const std::size_t region = RegionIndex(id);
    const RegionShopData& shop = GetRegionShopData(id);

    for (std::size_t slot = 0; slot < kShopSlotCount; ++slot) {
        adventure.shop_stock[region][slot] = shop.stock[slot].base_quantity;
    }
    adventure.shop_refresh_stage[region] = StoryStageIndex(adventure.story_stage);
}

void RefreshAllShopStock(AdventureState& adventure) {
    for (RegionId id : GetAllRegions()) {
        RefreshShopStock(adventure, id);
    }
}

void EnsureShopStockFresh(AdventureState& adventure, RegionId id) {
    const std::size_t region = RegionIndex(id);
    if (adventure.shop_refresh_stage[region] !=
        StoryStageIndex(adventure.story_stage)) {
        RefreshShopStock(adventure, id);
    }
}
