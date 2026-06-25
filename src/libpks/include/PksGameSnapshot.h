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

    // Current dice roll
    PksDicePair diceValues;
    std::pair<bool, bool> isDiceUsable;

    // Snitch feature
    std::set<PksPieceIdx> snitchablePieces;
    PksDMoveSet optimalMoves;
};

#endif //LIBPKS_PKSGAMESNAPSHOT_H
