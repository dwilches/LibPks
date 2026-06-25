#include <functional>
#include <ranges>

#include "PksConstants.h"
#include "PksException.h"
#include "PksGame.h"


struct CPksDiceResult {
    int dice1;
    int dice2;
    bool allDiceUsed;
    bool error;
};

struct CPksGameSnapshot {
    int piecesByPlayer[NUM_PLAYERS * NUM_PIECES];
    int currentPlayer;
    int gameState;
    //std::set<PksPieceIdx> snitchablePieces;
    //PksDMoveSet optimalMoves;
    bool error;
};

typedef void (*CPksLogCallback)(const char *message);

static CPksLogCallback pksLogCallback;
PksGame *pksGame;

CPksGameSnapshot toCStruct(const PksGameSnapshot &snapshot) {
    auto result = CPksGameSnapshot{
        .currentPlayer = static_cast<int>(snapshot.currentPlayer),
        .gameState = static_cast<int>(snapshot.gameState),
    };

    int i = 0;
    for (const auto &pieces: snapshot.piecesByPlayer | std::views::values) {
        for (const auto spotIdx: pieces) {
            result.piecesByPlayer[i++] = static_cast<int>(spotIdx);
        }
    }

    return result;
}

CPksDiceResult toCStruct(const PksDiceResult &diceResult) {
    const auto [dice1, dice2] = diceResult.getDice();
    return {
        .dice1 = dice1,
        .dice2 = dice2,
        .allDiceUsed = diceResult.allDiceUsed(),
    };
}

CPksGameSnapshot PksExecuteHandlingErrors(const std::function<PksGameSnapshot()> &func) {
    try {
        return toCStruct(func());
    } catch (PksException &e) {
        pksLogCallback(e.what());
        return {
            .error = true,
        };
    }
}

CPksDiceResult PksExecuteHandlingErrors(const std::function<PksDiceResult()> &func) {
    try {
        return toCStruct(func());
    } catch (PksException &e) {
        pksLogCallback(e.what());
        return {
            .error = true,
        };
    }
}

extern "C" {
// To cleanup resources created by these C-bindings, the user nees to invoke PksGameDestroy when it's done wiht libpks.
CPksGameSnapshot PksGameCreate() {
    return PksExecuteHandlingErrors([]() {
        pksGame = new PksGame;
        return pksGame->start();
    });
}

void PksGameDestroy() {
    delete pksGame;
}

// Invoked to roll 2 random dice
CPksDiceResult PksGameRollDice() {
    return PksExecuteHandlingErrors([]() {
        return pksGame->rollDice();
    });
}

// Moves a piece of the current player a certain number of spots
CPksGameSnapshot PksGameUseDice(const PksDiceVal diceVal,
                                const PksPieceIdx pieceIdx) {
    return PksExecuteHandlingErrors([diceVal, pieceIdx]() {
        return pksGame->useDice(diceVal, pieceIdx);
    });
}

// Returns the current location of every piece of the game, as well as whether the game has finished and who the
// current player is.
CPksGameSnapshot PksGetGameSnapshot() {
    return PksExecuteHandlingErrors([]() {
        return pksGame->getGameSnapshot();
    });
}

void PksRegisterLogger(const CPksLogCallback callback) {
    pksLogCallback = callback;
    pksLogCallback("CPksLogCallback registered successfully");
}
} // extern "C"
