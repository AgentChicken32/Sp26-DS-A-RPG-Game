#include "DialogueDatabase.h"
#include <array>
#include <filesystem>
#include <fstream>

//The Dialogue Database!!
std::unordered_map<std::string, std::unordered_map<int, TextNode>> dialogueData;


std::unordered_map<std::string, Type> TypeMap = {
	{"Statement", Type::Statement},
	{"Choice", Type::Choice}
};

static std::ifstream OpenDialogueJsonFile(std::filesystem::path& loadedPath) {
	const std::array<std::filesystem::path, 4> candidates = {
		"jsons/Dialogue.json",
		"../jsons/Dialogue.json",
		"../../jsons/Dialogue.json",
		"Dialogue.json"
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


bool LoadDialogueDatabase() {
	dialogueData.clear();

	std::filesystem::path loadedPath;
	std::ifstream file = OpenDialogueJsonFile(loadedPath);

	if (!file.is_open()) {
		std::cout << "File is not loaded! Failed to load action data!" << std::endl;
		std::cout << "Tried: jsons/Actions.json, ../jsons/Actions.json, ../../jsons/Actions.json, Actions.json" << std::endl;
		return false;
	}

	nlohmann::json data;
	try {
		file >> data;
	}
	catch (const std::exception& ex) {
		std::cout << "Failed to parse dialogue data JSON: " << ex.what() << std::endl;
		return false;
	}

	if (!data.is_array()) {
		std::cout << "Dialogue data JSON must be an array." << std::endl;
		return false;
	}

	std::size_t loadedCount = 0;

	//Fill database here
	for (const auto& group : data) {
			std::unordered_map<int, TextNode> script;

			//Fill Script wil text nodes
			for (const auto& text : group.at("Dialogue")) {
				TextNode T;
				T.id = text.at("id").get<int>();
				T.type = TypeMap.at(text.at("Type").get<std::string>());
				T.text = text.at("Text").get<std::string>();
				T.next = text.at("Next").get<int>();

				if (T.type == Type::Choice) {
					for (const auto& choice : text.at("Options")) {
						DialogueOption option;
						option.id = choice.at("id").get<int>();
						option.text = choice.at("Text").get<std::string>();
						option.next = choice.at("Next").get<int>();

						T.options.push_back(option);
					}
				}

				script[T.id] = T;
			}

			dialogueData[group.at("Group").get<std::string>()] = script;
			loadedCount++;
	}

	if (loadedCount == 0) {
		std::cout << "No valid text nodes were loaded from " << loadedPath << "." << std::endl;
		return false;
	}

	std::cout << "Loaded " << loadedCount << " dialogue from " << loadedPath << "." << std::endl;
	return true;
}


std::unordered_map<int, TextNode> GetDialogue(std::string group) {
	const auto& it = dialogueData.find(group);
	if (it != dialogueData.end()) {
		return it->second;
	}

	std::unordered_map<int, TextNode> script;
	TextNode T;
	T.id = -1;
	T.text = "Woah, secret message! Wait... this isn't a secret message... oh no! This is an error message!\n Error, non-existant group id: " + group;
	script[T.id] = T;
	return script;
}
