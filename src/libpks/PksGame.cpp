#include <sstream>

#include "PksGame.h"
#include "PksConstants.h"
#include "PksException.h"
#include "PksGameState.h"
#include "PksSpotType.h"
#include "PksUtils.h"

// This array implicitly establishes the sequence of player turns
static const std::vector playerColors{
    PksColor::Yellow, PksColor::Red, PksColor::Green, PksColor::Blue,
};

PksGame::PksGame(PksDiceRoller &diceRoller) : gameBoard{playerColors}, diceRoller{diceRoller} {
}

PksGameSnapshot PksGame::start() {
    if (gameState == PksGameState::GameInCourse) {
        throw PksException{"PksGame::start(): can't start a game that has already started."};
    }

    gameBoard.clear();
    currentPlayer = &playerColors[0];
    gameState = PksGameState::GameInCourse;

    return getGameSnapshot();
}

// Useful for tests
PksGameSnapshot PksGame::start(const PksGameSnapshot &gameSnapshot) {
    start();

    gameBoard.setPieces(gameSnapshot.piecesByPlayer);
    currentPlayer = &playerColors[static_cast<int>(gameSnapshot.currentPlayer)];

    return getGameSnapshot();
}

//TODO: do I need this?
void PksGame::stop() {
    validateGameInCourse("PksGame::stop()");

    gameBoard.clear();
    currentPlayer = &playerColors[0];
    gameState = PksGameState::GameNotStarted;
    currentPlayer = nullptr;
}

PksDiceResult PksGame::rollDice() {
    validateGameInCourse("PksGame::rollDice()");

    // Only roll the dice again if the previous one was already used
    if (lastRollDiceResult && !lastRollDiceResult->allDiceUsed()) {
        throw PksException{
            "PksGame::rollDice(): can't roll a new dice until all previous dice have been used."
        };
    }

    // Execute through a pointer for the polymorphic behaviour required by mocks in tests
    const auto diceRoll = (&diceRoller)->rollNewPair();

    lastRollDiceResult = std::make_unique<PksDiceResult>(diceRoll);
    numConsecutiveDiceRolls++;
    snitchablePieces = nullptr; // The ability to snitch on a player ends the next time dice are rolled

    if (lastRollDiceResult->isDoubles()) {
        // If the player doesn't have any piece at play, they need doubles before being allowed to use the dice.
        // If there was at least one piece at home, the dice is implicitly used to take them out of there.
        if (gameBoard.anyPieceAtSpot(*currentPlayer, HOME_SPOT)) {
            gameBoard.moveAllPiecesOutOfHome(*currentPlayer);

            // The dice roll cannot be used for anything else
            lastRollDiceResult->setDiceCannotBeUsed();

            // Capture all pieces that were walking by our Home
            moveHomeAllPiecesAtSpot(START_SPOT);

            nextPlayer();
        }

        // Doubles is good luck until it's 3 in a row, all pieces in play move back home
        if (numConsecutiveDiceRolls == MAX_DICE_ROLLS) {
            gameBoard.moveAllPlayingPiecesHome(*currentPlayer);
            // The player loses the possibility of using this dice
            lastRollDiceResult->setDiceCannotBeUsed();

            nextPlayer();
        }
    } else {
        // Can only use a non-doubles roll if there is at least 1 piece in play
        if (gameBoard.allPlayingPiecesAtHome(*currentPlayer)) {
            lastRollDiceResult->setDiceCannotBeUsed();

            if (numConsecutiveDiceRolls == MAX_DICE_ROLLS) {
                nextPlayer();
            }
        }
    }

    // If the user has an option to use dice, then using them unwisely can get them snitched
    if (!lastRollDiceResult->allDiceUsed()) {
        const auto &gameSnapshot = getGameSnapshot();
        snitchablePieces = std::make_unique<PksSnitcher>(gameSnapshot.piecesByPlayer,
                                                         *currentPlayer,
                                                         diceRoll);
    }

    // Return a copy of the dice result (we don't want the user to tamper with our library's internals)
    return *lastRollDiceResult;
}

void PksGame::nextPlayer() {
    const int colorIndex = (static_cast<int>(*currentPlayer) + 1) % NUM_PLAYERS;
    currentPlayer = &playerColors[colorIndex];
    numConsecutiveDiceRolls = 0;
}

PksGameSnapshot PksGame::useDice(const int diceValue, const int numPiece) {
    validateGameInCourse("PksGame::useDice()");

    if (numPiece < 0 || numPiece >= NUM_PIECES) {
        throw PksException{
            "PksGame::useDice(): Attempted to move invalid piece: " + std::to_string(numPiece)
        };
    }

    moveCurrentPlayerPiece(numPiece, diceValue);
    lastRollDiceResult->markDiceAsUsed(diceValue);

    const bool canRollAgain = lastRollDiceResult->isDoubles() && numConsecutiveDiceRolls != MAX_DICE_ROLLS;
    if (lastRollDiceResult->allDiceUsed() && !canRollAgain) {
        nextPlayer();
    }

    return getGameSnapshot();
}

// Returns true if the snitch was successful
bool PksGame::snitchOnPlayer(const PksColor &snitched, const std::set<PIECE_IDX> &pieces) {
    validateGameInCourse("PksGame::snitchOnPlayer()");

    if (!snitchablePieces || !snitchablePieces->isSnitchValid(snitched, pieces)) {
        return false;
    }

    // Successful snitch
    const auto snitchedColor = snitchablePieces->getSnitchablePlayer();
    gameBoard.movePiecesHome(snitchedColor, snitchablePieces->getSnitchablePieces());
    return true;
}

PksGameSnapshot PksGame::getGameSnapshot() const {
    return {
        .piecesByPlayer = gameBoard.getPieces(),
        .currentPlayer = *currentPlayer,
        .gameState = gameState,
        .snitchablePieces = snitchablePieces ? snitchablePieces->getSnitchablePieces() : std::set<PIECE_IDX>{},
    };
}

// Returns true if any piece was captured
bool PksGame::moveCurrentPlayerPiece(const int piece, const int numSpots) {
    // New position in player local numbering
    const int newPos = gameBoard.movePiece(*currentPlayer, piece, numSpots);

    const auto spotType = PksUtils::getSpotType(newPos);
    if (spotType == PksSpotType::Goal && gameBoard.allPiecesAtTarget(*currentPlayer)) {
        gameState = PksGameState::GameOver;
        return false;
    }

    // Check if this player's piece has fallen into an unsafe shared spot occupied by other players' pieces, and
    // capture them.
    if (spotType == PksSpotType::UnsafeShared) {
        return moveHomeAllPiecesAtSpot(newPos);
    }
    return false;
}

// Returns true if any piece was captured
bool PksGame::moveHomeAllPiecesAtSpot(const int spot) {
    bool anyPieceCaptured = false;
    for (const auto &otherColor: playerColors) {
        // Ignore the current player, only interested in other players
        if (otherColor == *currentPlayer) {
            continue;
        }

        const int otherPlayersSpot = PksUtils::convertSpotNumber(*currentPlayer, otherColor, spot);
        anyPieceCaptured = gameBoard.movePiecesHomeIfAtSpot(otherColor, otherPlayersSpot) || anyPieceCaptured;
    }
    return anyPieceCaptured;
}

void PksGame::validateGameInCourse(const std::string &methodName) const {
    if (gameState == PksGameState::GameInCourse) {
        return;
    }

    std::ostringstream oss;
    oss << methodName << ": Expected game to be in course, but it was [ " << gameState << "]";
    throw PksException{oss.str()};
}
