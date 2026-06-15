#ifndef LIBPKS_PKSDICEROLLER_H
#define LIBPKS_PKSDICEROLLER_H

#include <random>

class PksDiceRoller {
    std::default_random_engine generator;
    std::uniform_int_distribution<> distribution;

public:
    PksDiceRoller();

    virtual ~PksDiceRoller() = default;

    std::pair<int, int> rollNewPair();

    // Useful for mocking in tests
    virtual int nextRandomNumber();
};

#endif //LIBPKS_PKSDICEROLLER_H
