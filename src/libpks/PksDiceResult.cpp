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

bool PksDiceResult::isDiceAlreadyUsed(PksDiceVal diceValue) const {
    if (isDoubles()) {
        return isDiceUsed.first && isDiceUsed.second;
    }
    if (dicePair.first == diceValue) {
        return isDiceUsed.first;
    }
    if (dicePair.second == diceValue) {
        return isDiceUsed.second;
    }

    throw PksException{
        "There is no dice with value " + std::to_string(diceValue) + " in this roll."
    };
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
        // Should never happen because we validate the dice is not used before moving the piece, and this method
        // is invoked after moving the piece.
        throw PksException{
            "PksDiceResult::markDiceAsUsed(): Unexpected exception: dice with value " +
            std::to_string(diceValue) + " has already been used, and should have been validated before arriving at "
            "this line."
        };
    }
}
