#pragma once
#include "SceneClass.h"


class Battle : public Scene {
protected:
	//Members
	std::vector<int/*placeholder type, will be of type character pointer*/> enemies;
	std::vector<int/*placeholder type, will be of type character pointer*/> heroes;

	std::vector<int/*placeholder type, will be of type character pointer*/> turnOrder;

	int turnCounter = -1;//default to -1 to show it's the start of battle

public:
	//Methods

	Battle(std::vector<int/*character pointer*/> good, std::vector<int/*character pointer*/> evil);

	void Setup() override; //sets up scene, and displays it

	void DecideTurnOrder();//runs once in constructor to determine character action order

	void PlayerTurn(int/*placeholder for character pointer*/ npc);//player turn to choose attacks

	void EnemyTurn(int/*placeholder for character pointer*/ npc);//enemy automatically decides what to do

	int CheckForWinLoss();//checks if either team is at 0 health; returns 0 if neither team is dead, returns 1 if enemies are dead, and returns 2 if heroes are dead
};
