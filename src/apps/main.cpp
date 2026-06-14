#include <iostream>
#include <ostream>

#include "../libpks/include/PksGame.h"
#include "../libpks/include/PksDebug.h"

int main() {
    PksGame game;
    PksColor currentPlayer = game.start();
    std::cout << "currentPlayer " << currentPlayer << std::endl << std::endl;

    auto rollResult = game.rollDice();
    std::cout << "Roll Result is: " << *rollResult << std::endl << std::endl;

    std::cout << "Using dice 1: " << rollResult->diceRoll.dicePair.first.value << std::endl;
    auto nextPlayer = game.useDice(rollResult->diceRoll.dicePair.first.value, 0);
    std::cout << "nextPlayer=" << nextPlayer << std::endl << std::endl;

    std::cout << "Using dice 2:" << rollResult->diceRoll.dicePair.second.value << std::endl;
    nextPlayer = game.useDice(rollResult->diceRoll.dicePair.second.value, 1);
    std::cout << "nextPlayer=" << nextPlayer << std::endl << std::endl;

    game.rollDice();
}
