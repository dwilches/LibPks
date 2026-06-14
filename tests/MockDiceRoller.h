
#ifndef LIBPKS_MOCKDICEROLLER_H
#define LIBPKS_MOCKDICEROLLER_H

#include <deque>

#include "PksDiceRoller.h"

class MockDiceRoller : public PksDiceRoller {
    std::deque<int> randomSource;

public:
    void setNextRandomValues(int diceValue1, int diceValue2) {
        randomSource.push_back(diceValue1);
        randomSource.push_back(diceValue2);
    }

    int nextRandomNumber() override {
        const int nextValue = randomSource.front();
        randomSource.pop_front();
        return nextValue;
    }
};

#endif //LIBPKS_MOCKDICEROLLER_H
