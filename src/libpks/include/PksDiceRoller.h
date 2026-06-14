#ifndef LIBPKS_PKSDICEROLLER_H
#define LIBPKS_PKSDICEROLLER_H

#include <random>

struct RolledDice {
    int value;
    bool alreadyUsed;

    bool operator==(const RolledDice &other) const {
        return value == other.value && alreadyUsed == other.alreadyUsed;
    }
};

struct RolledDicePair {
    std::pair<RolledDice, RolledDice> dicePair;
    bool isDoubles;

    bool allDiceUsed() const;
};

class PksDiceRoller {
    std::default_random_engine generator;
    std::uniform_int_distribution<> distribution;

public:
    PksDiceRoller();

    PksDiceRoller(const PksDiceRoller &) = default;

    virtual ~PksDiceRoller() = default;

    RolledDicePair rollNewPair();

    // Useful for mocking in tests
    virtual int nextRandomNumber();
};

#endif //LIBPKS_PKSDICEROLLER_H
