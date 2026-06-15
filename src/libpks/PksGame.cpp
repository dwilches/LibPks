#include <sstream>

#include "PksGame.h"

#include <algorithm>

#include "PksConstants.h"
#include "PksException.h"
#include "PksGameState.h"
#include "PksSpotType.h"
#include "PksUtils.h"

static constexpr PksColor playerColors[] = {
    PksColor::Yellow, PksColor::Red, PksColor::Green, PksColor::Blue,
};

PksGame::PksGame(PksDiceRoller &diceRoller)
    : diceRoller{diceRoller},
      gameState{PksGameState::GameNotStarted},
      currentPlayer{PksColor::Yellow},
      numConsecutiveDiceRolls(0),
      lastRollDiceResult{nullptr} {
}

PksColor PksGame::start() {
    if (gameState == PksGameState::GameInCourse) {
        throw PksException{"PksGame::start(): can't start a game that has already started."};
    }

    players.clear();
    for (auto color: playerColors) {
        players.insert({color, {}});
    }

    gameState = PksGameState::GameInCourse;
    return currentPlayer = PksColor::Yellow;
}

// Useful for tests
PksColor PksGame::start(const PksBoardState &boardState) {
    if (gameState == PksGameState::GameInCourse) {
        throw PksException{"PksGame::start(): can't start a game that has already started."};
    }

    players.clear();
    for (const auto &[color, newPieces]: boardState.piecesByPlayer) {
        players.insert({color, PksPlayer{newPieces}});
    }

    gameState = PksGameState::GameInCourse;
    return currentPlayer = boardState.currentPlayer;
}

void PksGame::stop() {
    validateGameInCourse("PksGame::stop()");

    players.clear();
    gameState = PksGameState::GameNotStarted;
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
    auto diceRoll = (&diceRoller)->rollNewPair();

    lastRollDiceResult = std::make_unique<PksDiceResult>(PksDiceResult{diceRoll});
    numConsecutiveDiceRolls++;

    // If the player doesn't have any piece at play, they need doubles before being allowed to use the dice
    if (lastRollDiceResult->isDoubles()) {
        if (players.at(currentPlayer).anyPieceAtHome()) {
            // Implicitly use the dice roll to get our of Home
            players.at(currentPlayer).moveAllPiecesOutOfHome();
            // The dice roll cannot be used for anything else
            lastRollDiceResult->setDiceCannotBeUsed();

            // Capture all pieces that were walking by our Home
            moveHomeAllPiecesAtSpot(START_SPOT);

            nextPlayer();
        }
    } else {
        // Can only use a non-doubles roll if there is at least 1 piece in play
        if (players.at(currentPlayer).allPlayingPiecesAtHome()) {
            lastRollDiceResult->setDiceCannotBeUsed();
        }

        if (numConsecutiveDiceRolls == MAX_DICE_ROLLS) {
            nextPlayer();
        }
    }

    // Return a copy of the dice result (we don't want the user to tamper with our library's internals)
    return *lastRollDiceResult;
}

void PksGame::nextPlayer() {
    const int colorIndex = (static_cast<int>(currentPlayer) + 1) % NUM_PLAYERS;
    currentPlayer = static_cast<PksColor>(colorIndex);
    numConsecutiveDiceRolls = 0;
}

PksColor PksGame::useDice(const int diceValue, const int numPiece) {
    validateGameInCourse("PksGame::useDice()");

    if (numPiece < 0 || numPiece >= NUM_PIECES) {
        throw PksException{
            "PksGame::useDice(): Attempted to move invalid piece: " + std::to_string(numPiece)
        };
    }

    moveCurrentPlayerPiece(numPiece, diceValue);
    lastRollDiceResult->markDiceAsUsed(diceValue);

    if (lastRollDiceResult->allDiceUsed() && !(lastRollDiceResult->isDoubles() && numConsecutiveDiceRolls != MAX_DICE_ROLLS)) {
        nextPlayer();
    }

    return currentPlayer;
}

PksBoardState PksGame::getCurrentBoardState() const {
    std::map<PksColor, std::vector<int> > allPieces;
    for (auto &[color, player]: players) {
        allPieces[color] = player.getPieces();
    }
    return {
        .piecesByPlayer = allPieces,
        .currentPlayer = currentPlayer,
        .gameState = gameState
    };
}

void PksGame::moveCurrentPlayerPiece(int piece, int numSpots) {
    // New position in player local numbering
    const int newPos = players.at(currentPlayer).movePiece(piece, numSpots);

    const auto spotType = getSpotType(newPos);
    if (spotType == PksSpotType::Goal && players.at(currentPlayer).allPiecesAtTarget()) {
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
        if (otherColor == currentPlayer) {
            continue;
        }

        const int otherPlayersSpot = PksUtils::convertSpotNumber(currentPlayer, otherColor, spot);
        players.at(otherColor).movePiecesHomeIfAtSpot(otherPlayersSpot);
    }
}

PksSpotType PksGame::getSpotType(const int spot) {
    // Initial spot, treated as both Home and Jail
    if (spot == HOME_SPOT) { return PksSpotType::Home; }

    // Private stair, when the player gets here its pieces can't be eaten
    if (spot >= 63 && spot <= 70) {
        return PksSpotType::SafePrivate;
    }

    // Final spot, a piece can't move anymore after it has reached the target
    if (spot == FINAL_TARGET_SPOT) {
        return PksSpotType::Goal;
    }

    // Shared spots
    // Every 18 spots the types repeat, so simplify the spot number taking advantage of it being 0-indexed
    const int simplifiedSpot = spot % 17;
    if (simplifiedSpot == 0 || simplifiedSpot == 7 || simplifiedSpot == 12) {
        return PksSpotType::SafeShared;
    }

    return PksSpotType::UnsafeShared;
}

void PksGame::validateGameInCourse(const std::string &methodName) const {
    if (gameState == PksGameState::GameInCourse) {
        return;
    }

    std::ostringstream oss;
    oss << methodName << ": Expected game to be in course, but it was [ " << gameState << "]";
    throw PksException{oss.str()};
}
