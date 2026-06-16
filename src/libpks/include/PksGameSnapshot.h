#ifndef LIBPKS_PKSGAMESNPASHOT_H
#define LIBPKS_PKSGAMESNPASHOT_H

#include <map>
#include <vector>

#include "PksColor.h"
#include "PksGameState.h"

typedef std::map<PksColor, std::vector<int> > PksPiecesByPlayer;

struct PksGameSnapshot {
    PksPiecesByPlayer piecesByPlayer;
    PksColor currentPlayer = PksColor::Yellow;
    PksGameState gameState = PksGameState::GameNotStarted;
};

#endif //LIBPKS_PKSGAMESNPASHOT_H
