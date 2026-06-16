#ifndef LIBPKS_PKSDICEROLLER_H
#define LIBPKS_PKSDICEROLLER_H

#include <random>

/**
 * Small implementantion of a random dice.
 * This class allows mocking out the random numebrs source, making testing simple.
 */
class PksDiceRoller {
    std::default_random_engine generator;
    std::uniform_int_distribution<> distribution;

public:
    PksDiceRoller();

    virtual ~PksDiceRoller() = default;

    // Returns 2 numbers between 1 and 6, equivalent to rolling 2 dice in the game.
    std::pair<int, int> rollNewPair();

    // Useful for mocking in tests
    virtual int nextRandomNumber();
};

#endif //LIBPKS_PKSDICEROLLER_H
