#include <functional>
#include <ranges>

#include "PksConstants.h"
#include "PksException.h"
#include "PksGame.h"

struct CPksGameSnapshot {
    int piecesByPlayer[NUM_PLAYERS * NUM_PIECES];
    int currentPlayer;
    int gameState;

    // Current dice roll
    int diceValues[2];
    int isDiceUsable[2];

    // Snitch feature. Already implemented in LibPks but still not exposed as C bindings
    //std::set<PksPieceIdx> snitchablePieces;
    //PksDMoveSet optimalMoves;

    // If this field is true, callers should not attempt to access any of the other fields
    bool error;
};

typedef void (*CPksLogCallback)(const char *message);

static CPksLogCallback pksLogCallback;
PksGame *pksGame;

CPksGameSnapshot toCStruct(const PksGameSnapshot &snapshot) {
    CPksGameSnapshot result{};

    result.currentPlayer = static_cast<int>(snapshot.currentPlayer);
    result.gameState = static_cast<int>(snapshot.gameState);

    // Piece positions
    int i = 0;
    for (const auto &pieces: snapshot.piecesByPlayer | std::views::values) {
        for (const auto spotIdx: pieces) {
            result.piecesByPlayer[i++] = static_cast<int>(spotIdx);
        }
    }

    // Dice information
    result.diceValues[0] = snapshot.diceValues.first;
    result.diceValues[1] = snapshot.diceValues.second;
    result.isDiceUsable[0] = snapshot.isDiceUsable.first;
    result.isDiceUsable[1] = snapshot.isDiceUsable.second;

    return result;
}

CPksGameSnapshot PksExecuteHandlingErrors(const std::function<PksGameSnapshot()> &func) {
    try {
        return toCStruct(func());
    } catch (const PksException &e) {
        pksLogCallback(e.what());

        CPksGameSnapshot errorResult{};
        errorResult.error = true;
        return errorResult;
    }
}

extern "C" {
// To cleanup resources created by these C-bindings, the user nees to invoke PksGameDestroy when it's done wiht libpks.
CPksGameSnapshot PksGameCreate() {
    return PksExecuteHandlingErrors([] {
        pksGame = new PksGame;
        return pksGame->start();
    });
}

void PksGameDestroy() {
    delete pksGame;
    pksGame = nullptr;
}

// Invoked to roll 2 random dice
CPksGameSnapshot PksGameRollDice() {
    return PksExecuteHandlingErrors([] {
        pksGame->rollDice();
        return pksGame->getGameSnapshot();
    });
}

// Moves a piece of the current player a certain number of spots
CPksGameSnapshot PksGameUseDice(const PksDiceVal diceVal,
                                const PksPieceIdx pieceIdx) {
    return PksExecuteHandlingErrors([diceVal, pieceIdx] {
        return pksGame->useDice(diceVal, pieceIdx);
    });
}

// Returns the current location of every piece of the game, as well as whether the game has finished and who the
// current player is.
CPksGameSnapshot PksGetGameSnapshot() {
    return PksExecuteHandlingErrors([] {
        return pksGame->getGameSnapshot();
    });
}

void PksRegisterLogger(const CPksLogCallback callback) {
    pksLogCallback = callback;
    pksLogCallback("CPksLogCallback registered successfully");
}
} // extern "C"
