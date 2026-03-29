#include "WorldMap.h"

#include <stdexcept>

namespace {

constexpr std::size_t RegionIndex(RegionId id) {
    return static_cast<std::size_t>(id);
}

const std::array<RegionData, kRegionCount> kRegions = {{
    {RegionId::IceCourt, "Ice Court",
     "A frozen fortress at the roof of the world, where the wind cuts like glass.",
     4},
    {RegionId::Gelt, "Gelt",
     "A lonely northern coast of traders and smugglers watching the icepack drift.",
     1},
    {RegionId::NorthernWilds, "Northern Wilds",
     "Dense pine country full of old trails, hunters, and things that prefer twilight.",
     2},
    {RegionId::River, "River",
     "The great inland artery where barges, ferries, and hungry reeds crowd the banks.",
     1},
    {RegionId::Mudlands, "Mudlands",
     "Sodden lowlands where every path shifts beneath your boots.",
     3},
    {RegionId::Channel, "Channel",
     "A narrow split of water and stone that guides travelers toward the southern roads.",
     1},
    {RegionId::Glade, "Glade",
     "A hush of ancient trees and rune-marked standing stones that remember older bargains.",
     2},
    {RegionId::RuneMountains, "Rune Mountains",
     "Jagged peaks carved with glowing script and haunted by hard-bitten raiders.",
     3},
    {RegionId::PatomicCity, "Patomic City",
     "A harbor city of towers, gears, and restless markets built around an aging clockwork ward.",
     0},
    {RegionId::CentaurionPlaines, "Centaurion Plaines",
     "Open grassland where the horizon runs forever and hoofbeats carry for miles.",
     1},
    {RegionId::SouthernExpanse, "Southern Expanse",
     "A warm and wind-beaten stretch of coast where the road begins to feel forgotten.",
     2},
    {RegionId::ShrineOfTheWatchmaker, "Shrine of the Watchmaker",
     "An old sea shrine whose mechanisms still whisper when the tides are quiet.",
     0},
    {RegionId::EasternSea, "Eastern Sea",
     "A wide and mercurial sea, beautiful at dawn and dangerous by noon.",
     3},
    {RegionId::StormSpiralIsles, "Storm / Spiral Isles",
     "A ring of storm-torn isles circling strange waters and stranger lights.",
     4},
    {RegionId::EasternMountainChain, "Eastern Mountain Chain",
     "Black coastal cliffs and knife-edged passes scoured by sea-born thunder.",
     4},
    {RegionId::BlinkeringIsle, "Blinkering Isle",
     "A lonely island lighthouse blinking against the pull of spiral currents.",
     3},
}};

} // namespace

AdventureState CreateNewAdventure() {
    AdventureState adventure;
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

std::vector<RegionId> GetConnectedRegions(RegionId id) {
    switch (id) {
    case RegionId::IceCourt:
        return {RegionId::Gelt, RegionId::NorthernWilds};
    case RegionId::Gelt:
        return {RegionId::IceCourt};
    case RegionId::NorthernWilds:
        return {RegionId::IceCourt, RegionId::River, RegionId::EasternSea};
    case RegionId::River:
        return {RegionId::NorthernWilds, RegionId::Mudlands, RegionId::Channel};
    case RegionId::Mudlands:
        return {RegionId::River, RegionId::RuneMountains};
    case RegionId::Channel:
        return {RegionId::River, RegionId::Glade};
    case RegionId::Glade:
        return {RegionId::Channel, RegionId::RuneMountains};
    case RegionId::RuneMountains:
        return {RegionId::Glade, RegionId::Mudlands, RegionId::PatomicCity,
                RegionId::EasternSea};
    case RegionId::PatomicCity:
        return {RegionId::RuneMountains, RegionId::CentaurionPlaines,
                RegionId::SouthernExpanse, RegionId::EasternSea};
    case RegionId::CentaurionPlaines:
        return {RegionId::PatomicCity, RegionId::SouthernExpanse,
                RegionId::EasternSea};
    case RegionId::SouthernExpanse:
        return {RegionId::PatomicCity, RegionId::CentaurionPlaines,
                RegionId::ShrineOfTheWatchmaker, RegionId::EasternSea};
    case RegionId::ShrineOfTheWatchmaker:
        return {RegionId::SouthernExpanse};
    case RegionId::EasternSea:
        return {RegionId::NorthernWilds, RegionId::RuneMountains,
                RegionId::PatomicCity, RegionId::CentaurionPlaines,
                RegionId::SouthernExpanse, RegionId::StormSpiralIsles,
                RegionId::EasternMountainChain, RegionId::BlinkeringIsle};
    case RegionId::StormSpiralIsles:
        return {RegionId::EasternSea, RegionId::EasternMountainChain};
    case RegionId::EasternMountainChain:
        return {RegionId::EasternSea, RegionId::StormSpiralIsles,
                RegionId::BlinkeringIsle};
    case RegionId::BlinkeringIsle:
        return {RegionId::EasternSea, RegionId::EasternMountainChain};
    case RegionId::Count:
        break;
    }

    return {};
}

const std::array<RegionId, kRegionCount>& GetAllRegions() {
    static const std::array<RegionId, kRegionCount> kAllRegions = {{
        RegionId::IceCourt,
        RegionId::Gelt,
        RegionId::NorthernWilds,
        RegionId::River,
        RegionId::Mudlands,
        RegionId::Channel,
        RegionId::Glade,
        RegionId::RuneMountains,
        RegionId::PatomicCity,
        RegionId::CentaurionPlaines,
        RegionId::SouthernExpanse,
        RegionId::ShrineOfTheWatchmaker,
        RegionId::EasternSea,
        RegionId::StormSpiralIsles,
        RegionId::EasternMountainChain,
        RegionId::BlinkeringIsle,
    }};

    return kAllRegions;
}

const char* RegionName(RegionId id) {
    return GetRegionData(id).name;
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
