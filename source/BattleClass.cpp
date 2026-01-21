#include "BattleClass.h"
#include "character.h"
#include <vector>

using namespace std;

Battle::Battle(vector<character*> good, vector<character*> evil)
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
		character* actor = turnOrder[turnCounter];

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

	//Destroy Battle Scene
	//~Battle();
}

void Battle::DecideTurnOrder() {
	// Minimal stub: heroes go first, then enemies.
	turnOrder.clear();

	//smush both groups into an unsorted vector
	vector<character*> unsortedVec = enemies;
	unsortedVec.insert(unsortedVec.end(), heroes.begin(), heroes.end());
	
	//sort unsorted vector
	for (character* npc : unsortedVec) {
		//put first npc into turn order
		if (turnOrder.empty() == true) { turnOrder.push_back(npc); }

		//sort enemies into vector
		for (int i = 0; i <= turnOrder.size(); i++) {
			//check if current index is less than the npc that is being checked
			if (turnOrder[i]->get_stamina() <= npc->get_stamina()) {

				//if it is lesser then we insert current npc into vector before the smaller one
				turnOrder.insert(turnOrder.begin() + i, npc);
			}
			else if (i == turnOrder.size()) {
				//if none are larger than the current npc insert at the end
				turnOrder.insert(turnOrder.end(), npc);
			}
		}
	}

	// If both are empty, prevent out-of-bounds access in Setup
	if (turnOrder.empty()) {
		turnCounter = 0;
	}
}

void Battle::PlayerTurn(character* npc) {
	// stub
	(void)npc;
	// TODO: implement menu + actions
}

void Battle::EnemyTurn(character* npc) {
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
