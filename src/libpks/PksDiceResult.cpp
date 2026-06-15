#include "PksDiceResult.h"

PksDiceResult::PksDiceResult(const std::pair<int, int> &dicePair)
    : dicePair{dicePair} {
}

std::pair<int, int> PksDiceResult::getDice() const {
    return dicePair;
}

bool PksDiceResult::isDoubles() const {
    return dicePair.first == dicePair.second;
}

bool PksDiceResult::allDiceUsed() const {
    return isDiceUsed.first && isDiceUsed.second;
}

void PksDiceResult::setDiceCannotBeUsed() {
    isDiceUsed = {true, true};
}

void PksDiceResult::markDiceAsUsed(const int diceValue) {
    if (dicePair.first != diceValue && dicePair.second != diceValue) {
        throw PksException{
            "There is no dice with value " + std::to_string(diceValue) + " in this roll."
        };
    }

    if (dicePair.first == diceValue && !isDiceUsed.first) {
        isDiceUsed.first = true;
    } else if (dicePair.second == diceValue && !isDiceUsed.second) {
        isDiceUsed.second = true;
    } else {
        throw PksException{"Dice with value " + std::to_string(diceValue) + " has already been used."};
    }
}
