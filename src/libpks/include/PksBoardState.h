#ifndef LIBPKS_PKSBOARDSTATE_H
#define LIBPKS_PKSBOARDSTATE_H
#include <map>
#include <vector>

#include "PksColor.h"

struct PksBoardState {
    std::map<PksColor, std::vector<int>> piecesByPlayer;
    PksColor currentPlayer;
};

#endif //LIBPKS_PKSBOARDSTATE_H
