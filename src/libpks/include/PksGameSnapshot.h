#ifndef LIBPKS_PKSGAMESNPASHOT_H
#define LIBPKS_PKSGAMESNPASHOT_H

#include <set>

#include "PksColor.h"
#include "PksGameState.h"
#include "PksTypeDefs.h"

struct PksGameSnapshot {
    PksPiecesByPlayer piecesByPlayer;
    PksColor currentPlayer = PksColor::Yellow;
    PksGameState gameState = PksGameState::GameNotStarted;
    std::set<PIECE_IDX> snitchablePieces;
};

#endif //LIBPKS_PKSGAMESNPASHOT_H
