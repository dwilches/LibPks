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

PksGame::PksGame()
    : gameBoard{playerColors} {
    this->diceRoller = std::make_shared<PksDiceRoller>();
}

// Used for testing. Allows mocking the Dice Roller to give the dice values the tests expect.
PksGame::PksGame(const std::shared_ptr<PksDiceRoller> &diceRoller)
    : gameBoard{playerColors},
      diceRoller{diceRoller} {
}

PksGameSnapshot PksGame::start() {
    if (gameState == PksGameState::GameInCourse) {
        throw PksException{"PksGame::start(): can't start a game that has already started."};
    }

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

PksDiceResult PksGame::rollDice() {
    validateGameInCourse("PksGame::rollDice()");

    // Only roll the dice again if the previous one was already used
    if (lastRollDiceResult && !lastRollDiceResult->allDiceUsed()) {
        throw PksException{
            "PksGame::rollDice(): can't roll a new dice until all previous dice have been used."
        };
    }

    lastRollDiceResult = std::make_unique<PksDiceResult>(diceRoller->rollNewPair());
    numConsecutiveDiceRolls++;
    snitcher = nullptr; // The ability to snitch on a player ends the next time dice are rolled

    if (lastRollDiceResult->isDoubles()) {
        // If the player doesn't have any piece at play, they need doubles before being allowed to use the dice.
        // If there was at least one piece at home, the dice is implicitly used to take them out of there.
        if (gameBoard.anyPieceAtSpot(*currentPlayer, HOME_SPOT)) {
            // If any piece was walking by our home, they're captured
            gameBoard.moveAllPiecesOutOfHome(*currentPlayer);

            // The dice roll cannot be used for anything else
            lastRollDiceResult->setDiceCannotBeUsed();

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
        auto possibleSnitcher = std::make_unique<PksSnitcher>(gameBoard, *currentPlayer, lastRollDiceResult->getDice());
        // If there is no optimal move, snitch is not possible
        if (!possibleSnitcher->getOptimalMoves().empty()) {
            snitcher = std::move(possibleSnitcher);
        }
    }

    // Return a copy of the dice result (we don't want the user to tamper with our library's internals)
    return *lastRollDiceResult;
}

void PksGame::nextPlayer() {
    const int colorIndex = (static_cast<int>(*currentPlayer) + 1) % NUM_PLAYERS;
    currentPlayer = &playerColors[colorIndex];
    numConsecutiveDiceRolls = 0;
}

PksGameSnapshot PksGame::useDice(const PksDiceVal diceValue, const PksPieceIdx pieceIdx) {
    validateGameInCourse("PksGame::useDice()");

    if (pieceIdx < 0 || pieceIdx >= NUM_PIECES) {
        throw PksException{
            "PksGame::useDice(): Attempted to move invalid piece: " + std::to_string(pieceIdx)
        };
    }

    moveCurrentPlayerPiece(pieceIdx, diceValue);
    lastRollDiceResult->markDiceAsUsed(diceValue);
    if (snitcher) {
        snitcher->reportDiceUsed(diceValue, pieceIdx);
    }

    const bool canRollAgain = lastRollDiceResult->isDoubles() && numConsecutiveDiceRolls != MAX_DICE_ROLLS;
    if (lastRollDiceResult->allDiceUsed() && !canRollAgain) {
        nextPlayer();
    }

    return getGameSnapshot();
}

// Returns true if the snitch was successful
bool PksGame::snitchOnPlayer(const PksColor &snitched, const std::set<PksPieceIdx> &pieces) {
    validateGameInCourse("PksGame::snitchOnPlayer()");

    if (!snitcher || !snitcher->isSnitchValid(snitched, pieces)) {
        return false;
    }

    // Successful snitch
    const auto snitchedColor = snitcher->getSnitchablePlayer();
    gameBoard.movePiecesHome(snitchedColor, snitcher->getSnitchablePieces());
    return true;
}

PksGameSnapshot PksGame::getGameSnapshot() const {
    return {
        .piecesByPlayer = gameBoard.getPieces(),
        .currentPlayer = *currentPlayer,
        .gameState = gameState,
        .snitchablePieces = snitcher ? snitcher->getSnitchablePieces() : std::set<PksPieceIdx>{},
        .optimalMoves = snitcher ? snitcher->getOptimalMoves() : PksDMoveSet{},
    };
}

// Returns true if any piece was captured
bool PksGame::moveCurrentPlayerPiece(const PksPieceIdx pieceIdx, const int numSpots) {
    const int currentSpotIdx = gameBoard.getSpotForPiece(*currentPlayer, pieceIdx);
    if (currentSpotIdx == FINAL_TARGET_SPOT || currentSpotIdx == HOME_SPOT) {
        throw PksException{
            "PksGame::moveCurrentPlayerPiece(): Attempted to move a piece that is out of play: " +
            std::to_string(pieceIdx)
        };
    }
    const int numCaptured = gameBoard.movePiece(*currentPlayer, pieceIdx, numSpots);
    const int newSpotIdx = gameBoard.getSpotForPiece(*currentPlayer, pieceIdx);

    if (PksUtils::getSpotType(newSpotIdx) == PksSpotType::Goal && gameBoard.allPiecesAtTarget(*currentPlayer)) {
        gameState = PksGameState::GameOver;
        return false;
    }

    return numCaptured > 0;
}

void PksGame::validateGameInCourse(const std::string &methodName) const {
    if (gameState == PksGameState::GameInCourse) {
        return;
    }

    std::ostringstream oss;
    oss << methodName << ": Expected game to be in course, but it was [" << gameState << "]";
    throw PksException{oss.str()};
}
