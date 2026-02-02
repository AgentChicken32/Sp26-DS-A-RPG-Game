#include "BattleClass.h"
#include "character.h"
#include <vector>
#include <random>

using namespace std;

//placeholder functions

//Basic Implimentation of a player attack function
void Battle::BasicPlayerAttack(Character* npc) {
	//variables
	int input = 0;
	bool loop = false;

	//do while loop to loop if input is invalid
	do {
		loop = false;

		cout << "*-------------------------------------------------------*" << endl;
		cout << "Who do you want to wack?" << endl;

		//output enemy names, health, and numbers
		for (int counter = 1; counter <= (enemies.size() + 1); counter++) {
			if (counter <= enemies.size()) {
				cout << counter << ". " << enemies[counter - 1]->get_name() << "[HP:" << enemies[counter - 1]->get_health() << "] ";
			}
			else {
				cout << counter << ". BACK" << endl;
			}
		}

		//take input from user
		cin >> input;

		if (input <= enemies.size() && input > 0) {

			if (enemies[input - 1]->get_health() != 0) {
				enemies[input - 1]->take_damage(20);
				cout << "*-------------------------------------------------------*" << endl;
				cout << "You hit " << enemies[input - 1]->get_name() << "!\nIt took 30 damage!" << endl;
			}
			else {
				loop = true;
			}
		}
		else {
			loop = true;
		}


	} while (loop);
}

//Basic Implimentation of player magic function
void Battle::BasicPlayerMagic(Character* npc) {
	//variables
	int input = 0;
	bool loop = false;

	//do while loop to loop if input is invalid
	do {
		loop = false;

		cout << "*-------------------------------------------------------*" << endl;
		cout << "Who do you want to blast with a fireball?" << endl;

		//output enemy names, health, and numbers
		for (int counter = 1; counter <= (enemies.size() + 1); counter++) {
			if (counter <= enemies.size()) {
				cout << counter << ". " << enemies[counter - 1]->get_name() << "[HP:" << enemies[counter - 1]->get_health() << "] ";
			}
			else {
				cout << counter << ". BACK" << endl;
			}
		}

		//take input from user
		cin >> input;

		//input if statement
		if (input <= enemies.size() && input > 0) {

			//check if enemy selected is dead or not
			if (enemies[input - 1]->get_health() != 0) {
				//if the enemy isnt dead, it takes 30 damage
				enemies[input - 1]->take_damage(30);
				//player spends mana
				npc->spend_mana(10);
				//output results
				cout << "*-------------------------------------------------------*" << endl;
				cout << "You hit " << enemies[input - 1]->get_name() << "!\nIt took 30 damage!"<< endl;
			}
			else {
				//if the enemy is dead loop
				loop = true;
			}
		}
		else {
			loop = true;
		}


	} while (loop);
}


Battle::Battle(vector<Character*> good, vector<Character*> evil)
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
		Character* actor = turnOrder[turnCounter];

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
	vector<Character*> unsortedVec = enemies;
	unsortedVec.insert(unsortedVec.end(), heroes.begin(), heroes.end());
	
	//sort unsorted vector
	for (Character* npc : unsortedVec) {
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

void Battle::PlayerTurn(Character* npc) {
	// stub
	(void)npc;
	// TODO: implement menu + actions

	//display whose turn it is and their current state
	cout << "*-------------------------------------------------------*" << endl;
	cout << "It is " << "[INSERT CHARACTER NAME HERE]" << "'s turn!" << endl;
	cout << "Health: " << npc->get_health() << " Mana: " << npc->get_mana() << endl;

	//variables
	int input = 0;
	bool loop = false;

	//do while loop that will loop if told to
	do {

		//output text and menu options
		cout << R"(
*-------------------------------------------------------*
Choose an action!
1. ATTACK	2. MAGIC	3. ITEMS	4. ESCAPE
)" << endl;

		//take input from user
		cin >> input;

		//switch case for menu options
		switch (input) {
		case 1:
			//run attack
			BasicPlayerAttack(npc);
			break;
		case 2:
			//run magic
			BasicPlayerMagic(npc);
			break;
		case 3:
			//open item menu
			AccessInventory();
			loop = true;
			break;
		case 4:
			//try to escape battle
			cout << R"(
*-------------------------------------------------------*
You tried to run away, but it failed!
)" << endl;

			loop = true;
			break;
		default:
			//loop
			loop = true;
		}

	} while (loop);
}

void Battle::EnemyTurn(Character* npc) {
	// stub
	(void)npc;
	// TODO: implement AI action selection

	//choose which character the monster will hit
	random_device rand;
	mt19937 gen(rand());
	uniform_int_distribution<> choose(1, heroes.size());
	uniform_int_distribution<> damage(20, 30);

	//random number
	int randNum = choose(gen);

	bool loop = false;
	do {
		//deal damage
		if (heroes[randNum]->get_health() != 0) {
			heroes[randNum]->take_damage(damage(gen));
		}
		else {
			randNum = choose(gen);
			loop = true;
		}

	} while (loop);

	//output results
	cout << "*-------------------------------------------------------*" << endl;
	cout << npc->get_name() << " wacked " << heroes[randNum]->get_name() << "!" << endl;
	cout << heroes[randNum]->get_name() << " has " << heroes[randNum]->get_health() << " health left!" << endl;
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
	cout << "*-------------------------------------------------------*" << endl;
	cout << "There is nothing in your bag..." << endl;
}

//dumb virtual function, need arguments >:(
void Battle::MenuOptions() {

	return;
}