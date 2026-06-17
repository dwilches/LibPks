#ifndef LIBPKS_PKSDICERESULT_H
#define LIBPKS_PKSDICERESULT_H

#include "PksException.h"

/**
 * This class represents an instance of a dice roll (it holds the value of each dice) and also keeps track of whether
 * each dice has been used.
 */
class PksDiceResult {

    // Contains the values of 2 dice, from 1-6
    std::pair<int, int> dicePair;

    // Whether the player already used each of the dice.
    std::pair<bool, bool> isDiceUsed;

public:
    explicit PksDiceResult(const std::pair<int, int> &dicePair);

    [[nodiscard]] std::pair<int, int> getDice() const;

    // Rolling 2 dice with the same value (a.k.a. doubles) has special properties in the game.
    [[nodiscard]] bool isDoubles() const;

    [[nodiscard]] bool allDiceUsed() const;

    // Sometimes dice are used as soon as they are rolled, in those cases, this method invalidates both dice aat once.
    // An example is when a non-doubles is rolled and the player has all pieces at home.
    void setDiceCannotBeUsed();

    // Marks a single dice as used (e.g. when a player uses a dice's value to move a piece(.)
    void markDiceAsUsed(int diceValue);
};

#endif //LIBPKS_PKSDICERESULT_H
