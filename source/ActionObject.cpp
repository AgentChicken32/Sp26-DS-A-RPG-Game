#include "ActionObject.h"
#include <fstream>
#include <array>
#include <filesystem>

//Holds every loaded action in the game
std::unordered_map<int, ActionData> actionDatabase;

//TypeMaps help properly convert strings to the correct enums
std::unordered_map<std::string, Category> categoryTypeMap = {
	{"Physical", Category::Physical},
	{"Magic", Category::Magic},
};
std::unordered_map<std::string, EffectType> effectTypeMap = {
	{"Damage", EffectType::Damage},
	{"Status", EffectType::Status},
	{"Buff", EffectType::Buff},
	{"Debuff", EffectType::Debuff},
	{"Heal", EffectType::Heal}
};
std::unordered_map<std::string, StatusCondition> statusTypeMap = {
	{"None", StatusCondition::None},
	{"Burn", StatusCondition::Burn},
	{"Poison", StatusCondition::Poison},
	{"Frozen", StatusCondition::Frozen}
};

static std::ifstream OpenActionJsonFile(std::filesystem::path& loadedPath) {
	const std::array<std::filesystem::path, 4> candidates = {
		"jsons/Actions.json",
		"../jsons/Actions.json",
		"../../jsons/Actions.json",
		"Actions.json"
	};

	for (const auto& path : candidates) {
		std::ifstream file(path);
		if (file.is_open()) {
			loadedPath = path;
			return file;
		}
	}

	return std::ifstream{};
}

bool LoadDataBase() {
	actionDatabase.clear();

	std::filesystem::path loadedPath;
	std::ifstream file = OpenActionJsonFile(loadedPath);

	if (!file.is_open()) {
		std::cout << "File is not loaded! Failed to load action data!" << std::endl;
		std::cout << "Tried: jsons/Actions.json, ../jsons/Actions.json, ../../jsons/Actions.json, Actions.json" << std::endl;
		return false;
	}

	nlohmann::json data;
	try {
		file >> data;
	} catch (const std::exception& ex) {
		std::cout << "Failed to parse action data JSON: " << ex.what() << std::endl;
		return false;
	}

	if (!data.is_array()) {
		std::cout << "Action data JSON must be an array." << std::endl;
		return false;
	}

	std::size_t loadedCount = 0;

	for (const auto& action : data) {
		try {
			ActionData A;
			A.id = action.at("id").get<int>();
			A.name = action.at("name").get<std::string>();
			A.category = categoryTypeMap.at(action.at("category").get<std::string>());
			A.manaCost = action.at("manaCost").get<int>();
			A.accuracy = action.at("accuracy").get<int>();

			for (const auto& effect : action.at("effects")) {
				EffectData E;
				E.type = effectTypeMap.at(effect.at("type").get<std::string>());
				E.power = effect.value("power", 0);
				E.status = statusTypeMap.at(effect.value("statusCondition", std::string("None")));
				E.afflictionChance = effect.value("afflictionChance", 1.0);

				A.effects.push_back(E);
			}

			actionDatabase[A.id] = A;
			loadedCount++;
		} catch (const std::exception& ex) {
			std::cout << "Skipping malformed action entry: " << ex.what() << std::endl;
		}
	}

	if (loadedCount == 0) {
		std::cout << "No valid actions were loaded from " << loadedPath << "." << std::endl;
		return false;
	}

	std::cout << "Loaded " << loadedCount << " actions from " << loadedPath << "." << std::endl;
	return true;
}

ActionData GetAction(int id) {
	const auto it = actionDatabase.find(id);
	if (it != actionDatabase.end()) {
		return it->second;
	}

	ActionData missing;
	missing.id = id;
	missing.name = "Unknown Action";
	missing.accuracy = 0;
	return missing;
}

int GetDamage(ActionData action) {
	for (const auto& effect : action.effects) {
		if (effect.type == EffectType::Damage) {
			return effect.power;
		}
	}

	return 0;
}

double CheckIfStatus(ActionData action) {
	for (const auto& effect : action.effects) {
		if (effect.type == EffectType::Status) {
			return effect.afflictionChance;
		}
	}

	return 0;
}
