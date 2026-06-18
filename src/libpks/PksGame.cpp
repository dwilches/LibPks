#include <sstream>

#include "PksGame.h"
#include "PksConstants.h"
#include "PksException.h"
#include "PksGameState.h"
#include "PksSpotType.h"
#include "PksUtils.h"

// This array implicitly establishes the sequence of player turns
static constexpr PksColor playerColors[] = {
    PksColor::Yellow, PksColor::Red, PksColor::Green, PksColor::Blue,
};

PksGame::PksGame(PksDiceRoller &diceRoller) : diceRoller{diceRoller} {
}

PksGameSnapshot PksGame::start() {
    if (gameState == PksGameState::GameInCourse) {
        throw PksException{"PksGame::start(): can't start a game that has already started."};
    }

    currentPlayer = &playerColors[0];
    players.clear();
    for (auto color: playerColors) {
        players.insert({color, {}});
    }

    gameState = PksGameState::GameInCourse;
    return getGameSnapshot();
}

// Useful for tests
PksGameSnapshot PksGame::start(const PksGameSnapshot &gameSnapshot) {
    start();

    currentPlayer = &playerColors[static_cast<int>(gameSnapshot.currentPlayer)];
    for (const auto &[color, newPieces]: gameSnapshot.piecesByPlayer) {
        players[color] = PksPlayer{newPieces};
    }

    return getGameSnapshot();
}

void PksGame::stop() {
    validateGameInCourse("PksGame::stop()");

    players.clear();
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

    lastRollDiceResult = std::make_unique<PksDiceResult>(PksDiceResult{diceRoll});
    numConsecutiveDiceRolls++;

    // If the player doesn't have any piece at play, they need doubles before being allowed to use the dice
    if (lastRollDiceResult->isDoubles()) {
        if (players.at(*currentPlayer).anyPieceAtHome()) {
            // Implicitly use the dice roll to get our of Home
            players.at(*currentPlayer).moveAllPiecesOutOfHome();
            // The dice roll cannot be used for anything else
            lastRollDiceResult->setDiceCannotBeUsed();

            // Capture all pieces that were walking by our Home
            moveHomeAllPiecesAtSpot(START_SPOT);

            nextPlayer();
        }
    } else {
        // Can only use a non-doubles roll if there is at least 1 piece in play
        if (players.at(*currentPlayer).allPlayingPiecesAtHome()) {
            lastRollDiceResult->setDiceCannotBeUsed();

            if (numConsecutiveDiceRolls == MAX_DICE_ROLLS) {
                nextPlayer();
            }
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

PksGameSnapshot PksGame::getGameSnapshot() const {
    PksPiecesByPlayer allPieces;
    for (auto &[color, player]: players) {
        allPieces[color] = player.getPieces();
    }
    return {
        .piecesByPlayer = allPieces,
        .currentPlayer = *currentPlayer,
        .gameState = gameState
    };
}

void PksGame::moveCurrentPlayerPiece(const int piece, const int numSpots) {
    // New position in player local numbering
    const int newPos = players.at(*currentPlayer).movePiece(piece, numSpots);

    const auto spotType = PksUtils::getSpotType(newPos);
    if (spotType == PksSpotType::Goal && players.at(*currentPlayer).allPiecesAtTarget()) {
        gameState = PksGameState::GameOver;
        return;
    }

    // Check if this player's piece has fallen into an unsafe shared spot occupied by other players' pieces, and
    // capture them.
    if (spotType == PksSpotType::UnsafeShared) {
        moveHomeAllPiecesAtSpot(newPos);
    }
}

void PksGame::moveHomeAllPiecesAtSpot(const int spot) {
    for (const auto &otherColor: playerColors) {
        // Ignore the current player, only interested in other players
        if (otherColor == *currentPlayer) {
            continue;
        }

        const int otherPlayersSpot = PksUtils::convertSpotNumber(*currentPlayer, otherColor, spot);
        players.at(otherColor).movePiecesHomeIfAtSpot(otherPlayersSpot);
    }
}


void PksGame::validateGameInCourse(const std::string &methodName) const {
    if (gameState == PksGameState::GameInCourse) {
        return;
    }

    std::ostringstream oss;
    oss << methodName << ": Expected game to be in course, but it was [ " << gameState << "]";
    throw PksException{oss.str()};
}
