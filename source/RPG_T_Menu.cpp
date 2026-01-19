#include <iostream>
using namespace std;

enum class GameState {
	TesterMenu,
	Inventory,
	Battle,
	Shop,
	CharacterCreation,
	Quit
};

GameState runMainMenu()
{
	GameState state = GameState::TesterMenu;

	while (state != GameState::Quit) {
		switch (state) {

		case GameState::TesterMenu:
            // TODO: implement TesterMenu
			break;

		case GameState::Inventory:
			// TODO: implement inventory scene
			break;

		case GameState::Battle:
			// TODO: implement battle scene
			break;

		case GameState::Shop:
			// TODO: implement shop scene
			break;

		case GameState::CharacterCreation:
			// TODO: implement character creation
			break;

		case GameState::Quit:
			// exit loop
			break;

		default:
			// safety net
			state = GameState::Quit;
			break;
		}
	}

	return GameState::Quit;
}

/*
#include <iostream>
#include <string>
using namespace std;

enum class GameState {
    TesterMenu,
    Inventory,
    Battle,
    Shop,
    Quit
};

GameState runTesterMenu() {
   cout << "\n=== TESTER MENU ===\n";
   cout << "1) Return to Game\n";
   cout << "2) Battle test\n";
   cout << "3) Manage Inventory\n";
   cout << "4) Enter Shop\n";
   cout << "5) Quit\n";
   cout << "Choice: ";
   int choice{};
   cin >> choice;

   switch (choice) {
   case 1: return GameState::TesterMenu;
   case 2: return GameState::Battle;
   case 3: return GameState::Inventory;
   case 4: return GameState::Shop;
   case 5: return GameState::Quit;
   default:
       cout << "Invalid choice.\n";
       return GameState::TesterMenu;
   }
}
//GameState runBattle();
//GameState runInventory();
//GameState runShop();

int main() {
    GameState state = GameState::TesterMenu;

    while (state != GameState::Quit) {
        switch (state) {
        case GameState::TesterMenu:
            state = runTesterMenu();
            break;
            //INSET 1
        }
    }
    cout << "Exiting game...\n";
    return 0;
}
*/
//INSET 1
        /*case GameState::CharacterCreation:
            state = runCharacterCreation();
            break;
        case GameState::Battle:
            state = runBattle();
            break;
        case GameState::Quit:
            break;*/