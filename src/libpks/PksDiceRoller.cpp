#include <random>
#include <algorithm>

#include "PksDiceRoller.h"

#include <iostream>

PksDiceRoller::PksDiceRoller()
    : generator(std::random_device{}()),
      distribution(1, 6) {
}

RolledDicePair PksDiceRoller::rollNewPair() {
    const auto dice1 = RolledDice{
        .value = this->nextRandomNumber(),
        .alreadyUsed = false,
    };
    const auto dice2 = RolledDice{
        .value = this->nextRandomNumber(),
        .alreadyUsed = false,
    };

    return {
        .dicePair = std::pair{dice1, dice2},
        .isDoubles = dice1.value == dice2.value,
    };
}

int PksDiceRoller::nextRandomNumber() {
    return distribution(generator);
}

bool RolledDicePair::allDiceUsed() const {
    return dicePair.first.alreadyUsed && dicePair.second.alreadyUsed;
}
