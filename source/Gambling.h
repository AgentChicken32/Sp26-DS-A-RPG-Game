#ifndef GAMBLING_H
#define GAMBLING_H

class Gambling {
public:
    static void showMenu(int& gold);

private:
    static void playSlots(int& gold);
    static void playRoulette(int& gold);
    static int getBetAmount(int gold);
};

#endif