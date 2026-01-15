#pragma once
#include <iostrean>
#include <string>
#include <vector>


class Scene {
protected:
	//Members
	std::string name = "Default";

public:
	//Methods

	//Important Functions
	virtual void Setup() = 0;//starts scene functions and functions

	Scene(std::string newName);//constructor

	//Basic Placeholder Functions
	virtual void AccessInventory() = 0;//in most scenes the player will be able to access their inventory which may be another scene

	virtual void MenuOptions() = 0;//basic display menu function

};

