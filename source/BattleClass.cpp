#include "BattleClass.h"
#include <iostream>

using namespace std;

Battle::Battle(std::vector<int/*character pointer*/> good, std::vector<int/*character pointer*/> evil)
	: Scene("Battle") // <-- required: Scene has no default constructor
{
	heroes = good;
	enemies = evil;

	DecideTurnOrder();

	// game starts
	Setup();
}

void Battle::Setup() {
	// check if it's the first turn; if it is display start message
	if (turnCounter == -1) {
		cout << "Stand guard, enemies appeared!" << endl;
		turnCounter = 0;
	}

	// MAIN BATTLE LOOP!!
	while (CheckForWinLoss() == 0) {

		// Determine whether current actor is an enemy or hero
		int actor = turnOrder[turnCounter];

		bool is_enemy = (std::find(enemies.begin(), enemies.end(), actor) != enemies.end());

		if (is_enemy) {
			EnemyTurn(actor);
		} else {
			PlayerTurn(actor);
		}

		// advance turn counter with wrap-around
		turnCounter++;
		if (turnCounter >= static_cast<int>(turnOrder.size())) {
			turnCounter = 0;
		}
	}

	// give winning or losing message
	if (CheckForWinLoss() == 1) {
		cout << "You win!" << endl;
	} else {
		cout << "You lose!" << endl;
	}
}

void Battle::DecideTurnOrder() {
	// Minimal stub: heroes go first, then enemies.
	turnOrder.clear();
	turnOrder.insert(turnOrder.end(), heroes.begin(), heroes.end());
	turnOrder.insert(turnOrder.end(), enemies.begin(), enemies.end());

	// If both are empty, prevent out-of-bounds access in Setup
	if (turnOrder.empty()) {
		turnCounter = 0;
	}
}

void Battle::PlayerTurn(int/*placeholder for character pointer*/ npc) {
	// stub
	(void)npc;
	// TODO: implement menu + actions
}

void Battle::EnemyTurn(int/*placeholder for character pointer*/ npc) {
	// stub
	(void)npc;
	// TODO: implement AI action selection
}

int Battle::CheckForWinLoss() {
	// TODO: health checks once "character pointer" is a real type
	// For now, keep battle "ongoing" unless one side is empty.

	if (enemies.empty()) return 1;
	if (heroes.empty()) return 2;
	return 0;
}

void Battle::AccessInventory() {
	// stub (required by Scene)
}

void Battle::MenuOptions() {
	// stub (required by Scene)
}
