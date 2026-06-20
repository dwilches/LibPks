#include <iostream>
#include <ostream>
#include <vector>

#include "PksGame.h"

void printPositions(const std::vector<int> &);

int main() {
    std::cout << "/**" << std::endl;
    std::cout << " * This is a sample interaction with libpks." << std::endl;
    std::cout << " * This library can be used as the brain of your UI Parques implementations." << std::endl;
    std::cout << " */" << std::endl << std::endl;

    PksGame game;
    auto gameState = game.start();
    std::cout << "CurrentPlayer is " << gameState.currentPlayer << std::endl;

    std::cout << "This player's pieces are all at home (-1 is Home): " << std::endl;
    printPositions(gameState.piecesByPlayer.at(gameState.currentPlayer));

    std::cout << std::endl << "Rolling some dice" << std::endl;
    auto [dice1, dice2] = game.rollDice().getDice();
    std::cout << "Roll Result is: (" << dice1 << "," << dice2 << ")" << std::endl << std::endl;

    if (dice1 == dice2) {
        std::cout << "How lucky, we got doubles. We get out of Home" << std::endl;
    } else {
        std::cout << "We didn't get doubles, so we can't go out of Home." << std::endl;
    }

    std::cout << "New piece positions: " << std::endl;
    gameState = game.getGameSnapshot();
    printPositions(gameState.piecesByPlayer.at(gameState.currentPlayer));
}

void printPositions(const std::vector<int> &piecePositions) {
    for (int i = 0; i < piecePositions.size(); i++) {
        std::cout << "Piece [" << i << "] = " << piecePositions[i] << std::endl;
    }
}
