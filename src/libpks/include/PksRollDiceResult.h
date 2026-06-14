//
// Created by dwilches on 6/13/26.
//

#ifndef LIBPKS_PKSROLLDICERESULT_H
#define LIBPKS_PKSROLLDICERESULT_H
#include "PksException.h"

struct PksRollDiceResult {
    RolledDicePair diceRoll;
    bool canUseDiceRoll = false;
    bool canRollAgain = false;

    void markDiceAsUsed(int diceValue);
};

//TODO:
inline void PksRollDiceResult::markDiceAsUsed(int diceValue) {
    if (diceRoll.dicePair.first.value != diceValue && diceRoll.dicePair.second.value != diceValue) {
        throw PksException{
            "There is no dice with value " + std::to_string(diceValue) + " in this roll."
        };
    }

    if (diceRoll.dicePair.first.value == diceValue && !diceRoll.dicePair.first.alreadyUsed) {
        diceRoll.dicePair.first.alreadyUsed = true;
    } else if (diceRoll.dicePair.second.value == diceValue && !diceRoll.dicePair.second.alreadyUsed) {
        diceRoll.dicePair.second.alreadyUsed = true;
    } else {
        throw PksException{"Dice with value " + std::to_string(diceValue) + " has already been used."};
    }
}

#endif //LIBPKS_PKSROLLDICERESULT_H
