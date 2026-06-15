#include <random>
#include <algorithm>

#include "PksDiceRoller.h"

PksDiceRoller::PksDiceRoller()
    : generator(std::random_device{}()),
      distribution(1, 6) {
}

std::pair<int, int> PksDiceRoller::rollNewPair() {
    const auto dice1 = this->nextRandomNumber();
    const auto dice2 = this->nextRandomNumber();
    return {dice1, dice2};
}

int PksDiceRoller::nextRandomNumber() {
    return distribution(generator);
}
