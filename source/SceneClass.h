#pragma once
#include <iostream>
#include <string>
#include <vector>

class Scene {
protected:
	//Members
	std::string name = "Default";

public:
	//Methods

	// IMPORTANT: polymorphic base class should have a virtual destructor
	virtual ~Scene() = default;

	//Important Functions
	virtual void Setup() = 0;//starts scene functions and functions

	Scene(std::string newName);//constructor

	//Basic Placeholder Functions
	virtual void AccessInventory() = 0;//in most scenes the player will be able to access their inventory which may be another scene

	virtual void MenuOptions() = 0;//basic display menu function
};

/*
#pragma once
#include "SceneClass.h"
#include <algorithm> // std::find

class Battle : public Scene {
protected:
	// Members
	std::vector<int> enemies;   // placeholder type, will be of type character pointer
	std::vector<int> heroes;    // placeholder type, will be of type character pointer

	std::vector<int> turnOrder; // placeholder type, will be of type character pointer

	int turnCounter = -1; // default to -1 to show it's the start of battle

public:
	// Methods
	Battle(std::vector<int> good, std::vector<int> evil); // character pointer placeholders

	void Setup() override; // sets up scene and displays it

	// Required because Scene declares them pure virtual
	void AccessInventory() override;
	void MenuOptions() override;

	void DecideTurnOrder(); // runs once in constructor to determine character action order

	void PlayerTurn(int npc); // placeholder for character pointer, player turn to choose attacks
	void EnemyTurn(int npc);  // placeholder for character pointer, enemy automatically decides what to do

	int CheckForWinLoss(); // 0 = ongoing, 1 = enemies dead, 2 = heroes dead
};
*/