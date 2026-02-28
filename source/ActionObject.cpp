#include "ActionObject.h"
#include <fstream>

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
	{"Debuff", EffectType::Debuff}
};
std::unordered_map<std::string, StatusCondition> statusTypeMap = {
	{"None", StatusCondition::None},
	{"Burn", StatusCondition::Burn},
	{"Poison", StatusCondition::Poison}
};

void LoadDataBase() {
	//std::cout << "Running..." << std::endl;

	//variable for json file
	std::ifstream file("Actions.json");

	//check if the file is open
	if (!file.is_open()) {
		std::cout << "File is not loaded! Failed to load action data!" << std::endl;
		return;
	}

	//convert file variable into json variable
	nlohmann::json data;
	file >> data;
	
	//LARGE BLOCK OF CODE
	//this block of code iterates through the json file and fills the actionDatabase
	for (const auto& action : data) {
		//this section fills the actions variables
		ActionData A;
		A.id = action["id"].get<int>();
		A.name = action["name"].get<std::string>();
		A.category = categoryTypeMap.at(action["category"].get<std::string>());
		A.manaCost = action["manaCost"].get<int>();
		A.accuracy = action["accuracy"].get<int>();

		//this for loop fills the actions effect's variables then adds it to a vector
		for (const auto& effect : action["effects"]) {
			EffectData* E = new EffectData();
			E->type = effectTypeMap.at(effect["type"].get<std::string>());
			E->power = effect.value("power", 0);
			E->status = statusTypeMap.at(effect.value("statusCondition", ("None")));
			E->afflictionChance = effect.value("afflictionChance", 1.0);

			A.effects.push_back(*E);
		}

		//place action into database
		actionDatabase[A.id] = A;

		//Debugging
		//std::cout << "Loaded " << actionDatabase[A.id].name << "!" << std::endl;
	}

	//std::cout << "Ending..." << std::endl;
}

ActionData GetAction(int id) {
	return actionDatabase[1];
}