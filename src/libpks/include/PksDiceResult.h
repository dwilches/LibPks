#ifndef LIBPKS_PKSDICERESULT_H
#define LIBPKS_PKSDICERESULT_H

#include "PksException.h"

class PksDiceResult {
    std::pair<int, int> dicePair;
    std::pair<bool, bool> isDiceUsed;

public:
    explicit PksDiceResult(const std::pair<int, int> &dicePair);

    [[nodiscard]] std::pair<int, int> getDice() const;

    [[nodiscard]] bool isDoubles() const;

    [[nodiscard]] bool allDiceUsed() const;

    void setDiceCannotBeUsed();

    void markDiceAsUsed(int diceValue);
};

#endif //LIBPKS_PKSDICERESULT_H
