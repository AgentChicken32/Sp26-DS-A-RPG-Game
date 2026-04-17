#include "Gambling.h"

#include <iostream>
#include <cstdlib>
#include <ctime>

void Gambling::showMenu(int& gold) {
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }

    int choice;

    do {
        std::cout << "\n=== Casino ===\n";
        std::cout << "Gold: " << gold << "\n";
        std::cout << "1. Slot Machine\n";
        std::cout << "2. Roulette\n";
        std::cout << "3. Leave\n";
        std::cout << "Choose an option: ";
        std::cin >> choice;

        switch (choice) {
        case 1:
            playSlots(gold);
            break;
        case 2:
            playRoulette(gold);
            break;
        case 3:
            std::cout << "You leave the gambling hall.\n";
            break;
        default:
            std::cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 3);
}

int Gambling::getBetAmount(int gold) {
    int bet;

    if (gold <= 0) {
        std::cout << "You have no gold to bet.\n";
        return 0;
    }

    std::cout << "Enter bet amount: ";
    std::cin >> bet;

    if (bet <= 0 || bet > gold) {
        std::cout << "Invalid bet amount.\n";
        return 0;
    }

    return bet;
}

void Gambling::playSlots(int& gold) {
    int bet = getBetAmount(gold);
    if (bet == 0) {
        return;
    }

    int reel1 = std::rand() % 3 + 1;
    int reel2 = std::rand() % 3 + 1;
    int reel3 = std::rand() % 3 + 1;

    std::cout << "Slots: [" << reel1 << "] [" << reel2 << "] [" << reel3 << "]\n";

    if (reel1 == reel2 && reel2 == reel3) {
        int winnings = bet * 3;
        gold += winnings;
        std::cout << "Jackpot! You win " << winnings << " gold!\n";
    }
    else if (reel1 == reel2 || reel2 == reel3 || reel1 == reel3) {
        int winnings = bet;
        gold += winnings;
        std::cout << "Two matched! You win " << winnings << " gold!\n";
    }
    else {
        gold -= bet;
        std::cout << "No match. You lose " << bet << " gold.\n";
    }
}

void Gambling::playRoulette(int& gold) {
    int bet = getBetAmount(gold);
    if (bet == 0) {
        return;
    }

    int playerChoice;
    std::cout << "Pick a color: 1 for Red, 2 for Black: ";
    std::cin >> playerChoice;

    if (playerChoice != 1 && playerChoice != 2) {
        std::cout << "Invalid choice.\n";
        return;
    }

    int result = std::rand() % 2 + 1;
    std::cout << "Roulette landed on " << (result == 1 ? "Red" : "Black") << ".\n";

    if (playerChoice == result) {
        gold += bet;
        std::cout << "You guessed right and won " << bet << " gold!\n";
    }
    else {
        gold -= bet;
        std::cout << "Wrong guess. You lose " << bet << " gold.\n";
    }
}
