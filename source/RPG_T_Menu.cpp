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
        /*case GameState::CharacterCreation:
            state = runCharacterCreation();
            break;
        case GameState::Battle:
            state = runBattle();
            break;
        case GameState::Quit:
            break;*/
        }
    }
    cout << "Exiting game...\n";
    return 0;
}