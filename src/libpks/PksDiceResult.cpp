#include "PksDiceResult.h"

#include "PksException.h"

PksDiceResult::PksDiceResult(const PksDicePair &dicePair)
    : dicePair{dicePair} {
}

PksDicePair PksDiceResult::getDice() const {
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

void PksDiceResult::markDiceAsUsed(const PksDiceVal diceValue) {
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
