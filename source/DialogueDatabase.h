#pragma once
#include "json.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

//DATATYPES
enum Type {
	Statement,
	Choice,
	Null
};

struct TextNode {
	int id = 0;
	Type type = Null;
	std::string text = "";
	int next = 0;
	std::vector<TextNode*> options;
};

//FUNCTIONS

//loads database with script objects that can be found with group ids, eg Conversation1
bool LoadDialogueDatabase();

//returns a script with inputted group id
std::unordered_map<int, TextNode> GetDialogue(std::string group);
