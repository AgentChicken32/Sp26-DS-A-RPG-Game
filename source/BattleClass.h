#pragma once
#include "SceneClass.h"
#include "character.h"
#include <vector>
#include <algorithm> // std::find

class Battle : public Scene {
protected:
	// Members
	std::vector<Character*> enemies;
	std::vector<Character*> heroes;

	std::vector<Character*> turnOrder;

	int turnCounter = -1; // default to -1 to show it's the start of battle

public:
	// Methods
	Battle(std::vector<Character*> good, std::vector<Character*> evil);

	void Setup() override; // sets up scene, and displays it

	// These two are required because Scene declares them pure virtual
	void AccessInventory() override; 
	void MenuOptions() override;

	void DecideTurnOrder(); // runs once in constructor to determine character action order

	void PlayerTurn(Character* npc); // player turn to choose attacks
	void EnemyTurn(Character* npc);  // enemy automatically decides what to do

	int CheckForWinLoss(); // 0 = ongoing, 1 = enemies dead, 2 = heroes dead
};
