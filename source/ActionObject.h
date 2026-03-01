#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "json.hpp"


enum Category {
	Physical,
	Magic
};

enum EffectType {
	Damage,
	Status,
	Buff,
	Debuff
};

enum class StatusCondition {
	None,
	Poison,
	Burn
};

struct EffectData {
	EffectType type = EffectType::Damage;
	StatusCondition status = StatusCondition::None;
	int power = 0;
	double afflictionChance = 0;
};

struct ActionData {
	int id = 0;
	std::string name = "null";
	Category category = Category::Physical;
	int manaCost = 0;
	int accuracy = 1;
	std::vector<EffectData> effects;
};

//holds all the data of each move
extern std::unordered_map<int, ActionData> actionDatabase;

//fills action database with all action data
bool LoadDataBase();

//get specified action from action data using action id
ActionData GetAction(int id);
