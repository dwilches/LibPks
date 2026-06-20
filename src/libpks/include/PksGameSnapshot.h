#ifndef LIBPKS_PKSGAMESNAPSHOT_H
#define LIBPKS_PKSGAMESNAPSHOT_H

#include <set>

#include "PksColor.h"
#include "PksGameState.h"
#include "PksMoves.h"
#include "PksTypeDefs.h"

struct PksGameSnapshot {
    PksPiecesByPlayer piecesByPlayer;
    PksColor currentPlayer = PksColor::Yellow;
    PksGameState gameState = PksGameState::GameNotStarted;
    std::set<PksPieceIdx> snitchablePieces;
    PksDMoveSet optimalMoves;
};

#endif //LIBPKS_PKSGAMESNAPSHOT_H
