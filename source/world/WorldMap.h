#pragma once

#include <array>
#include <cstddef>
#include <string>
#include <vector>

enum class RegionId {
    IceCourt,
    Gelt,
    NorthernWilds,
    River,
    Mudlands,
    Channel,
    Glade,
    RuneMountains,
    PatomicCity,
    CentaurionPlaines,
    SouthernExpanse,
    ShrineOfTheWatchmaker,
    EasternSea,
    StormSpiralIsles,
    EasternMountainChain,
    BlinkeringIsle,
    Casino,
    Count
};

constexpr std::size_t kRegionCount = static_cast<std::size_t>(RegionId::Count);
constexpr std::size_t kShopSlotCount = 4;

struct ShopStockEntry {
    const char* item_name = "";
    int price = 0;
    int base_quantity = 0;
};

struct RegionShopData {
    const char* name = "";
    std::array<ShopStockEntry, kShopSlotCount> stock{};
};

enum class RegionSection {
    North,
    WesternMainland,
    EasternReach,
    Count
};

constexpr std::size_t kRegionSectionCount =
    static_cast<std::size_t>(RegionSection::Count);

enum class StoryStage {
    Prologue,
    SeekGlade,
    SeekShrine,
    ReturnToPatomic,
    DefendPatomic,
    Epilogue
};

struct AdventureState {
    RegionId current_region = RegionId::PatomicCity;
    StoryStage story_stage = StoryStage::Prologue;
    std::array<bool, kRegionCount> visited{};
    std::array<int, kRegionCount> shop_refresh_stage{};
    std::array<std::array<int, kShopSlotCount>, kRegionCount> shop_stock{};
};

struct ExplorationItemProfile {
    const char* first_item = "Potion";
    const char* second_item = "Herb";
    int first_item_chance = 70;
};

struct RegionData {
    RegionId id = RegionId::PatomicCity;
    const char* name = "";
    const char* description = "";
    int danger_level = 0;
    RegionSection section = RegionSection::WesternMainland;
    std::array<const char*, 2> enemy_names = {{ "Wandering Foe", "Wandering Foe" }};
    ExplorationItemProfile exploration_items{};
};

std::array<int, 6> GetEnemyActions(const std::string& name);
AdventureState CreateNewAdventure();
const RegionData& GetRegionData(RegionId id);
const RegionShopData& GetRegionShopData(RegionId id);
std::vector<RegionId> GetConnectedRegions(RegionId id);
const std::array<RegionId, kRegionCount>& GetAllRegions();
const std::array<RegionSection, kRegionSectionCount>& GetRegionSections();
const char* RegionName(RegionId id);
const char* RegionSectionName(RegionSection section);
int StoryStageIndex(StoryStage stage);
const char* StoryStageName(StoryStage stage);
const char* StoryObjective(const AdventureState& adventure);
bool TryParseRegionId(const std::string& value, RegionId& out);
bool TryParseStoryStage(const std::string& value, StoryStage& out);
bool HasVisited(const AdventureState& adventure, RegionId id);
void MarkVisited(AdventureState& adventure, RegionId id);
void RefreshShopStock(AdventureState& adventure, RegionId id);
void RefreshAllShopStock(AdventureState& adventure);
void EnsureShopStockFresh(AdventureState& adventure, RegionId id);
