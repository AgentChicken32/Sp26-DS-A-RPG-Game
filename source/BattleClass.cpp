#include "BattleClass.h"

using namespace std;

Battle::Battle(std::vector<int/*character pointer*/> good, std::vector<int/*character pointer*/> evil) {
	heroes = good;
	enemies = evil;
	//doesn't do anything yet, will input good and evil vectors to put into turn order vector
	DecideTurnOrder();

	//game starts
	Setup();
}

Battle::Setup() {
	//check if it's the first turn; if it is display start message
	if (turnCounter == -1) {
		cout << "Stand guard, enemies appeared!" << endl;
		turnCounter++;
	}

	//MAIN BATTLE LOOP!!
	while (CheckForWinLoss == 0) {
		//check whose turn it is
		if (/*check if turn counter correlates to an enemy in the turn order vector */ ) {
			EnemyTurn(turnOrder[turnCounter]);
		}
		else {//else its a player character
			PlayerTurn(turnOrder[turnCounter]);
		}
		
		//turn loop check
		if (turnCounter == turnOrder.size()) {
			turnCounter = 0
		}
		else {
			turnCounter++
		}
	}
	
	//give winning or losing message
	if (CheckForWinLoss == 1) {
		//1 means enemies are dead
		cout << "You win!"
	}
	else {
		//2 means heroes are dead
		cout << "You lose!"
	}
}

Battle::DecideTurnOrder() {
	//sort hero vector into turn order vector

	//sort enemy vector into turn order vector
}

Battle::PlayerTurn(int/*placeholder for character pointer*/ npc) {
	//display which player character's turn it is

	//give you menu options; Attack, Magic, Inventory, Run

	//attack uses character strength stat to wack enemy
	
	//magic uses character wisdom stat to blast enemy

	//inventory displays items character can use in battle

	//run character attempts to escape

	//end turn if use an item, attack, magic, or attempt to run
}

Battle::EnemyTurn(int/*placeholder for character pointer*/ npc) {
	//decide target

	//call enemy attack function

	//display message

	//end turn
}

Battle::CheckForWinLoss() {
	//iterate through enemies and check if their total health is 0; if it is return 1

	//iterate through heroes and check if their total health is 0; if it is return 2

	//if neither
	return 0;
}