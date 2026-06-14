#include <stdexcept>

#include "PksGame.h"

#include <algorithm>

#include "PksConstants.h"
#include "PksException.h"
#include "PksGameState.h"
#include "PksSpotType.h"
#include "PksUtils.h"

const PksColor playerColors[] = {
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
    validateGameState("PksGame::start()", PksGameState::GameNotStarted);

    players = {
        {PksColor::Yellow, {1, PksColor::Yellow}},
        {PksColor::Red, {2, PksColor::Red}},
        {PksColor::Green, {3, PksColor::Green}},
        {PksColor::Blue, {4, PksColor::Blue}},
    };

    gameState = PksGameState::GameInCourse;
    currentPlayer = PksColor::Yellow;
    return players.at(currentPlayer).getColor();
}

// Useful for tests
PksColor PksGame::start(const PksBoardState &boardState) {
    start();

    for (auto &[color, pieces]: boardState.piecesByPlayer) {
        players.at(color).setPieces(pieces);
    }
    currentPlayer = boardState.currentPlayer;

    return boardState.currentPlayer;
}

void PksGame::stop() {
    validateGameState("PksGame::stop()", PksGameState::GameInCourse);

    players.clear();
    gameState = PksGameState::GameNotStarted;
}

std::pair<RolledDice, RolledDice> PksGame::rollDice() {
    validateGameState("PksGame::rollDice()", PksGameState::GameInCourse);

    // Only roll the dice again if the previous one was already used
    if (lastRollDiceResult && lastRollDiceResult->canUseDiceRoll && !lastRollDiceResult->diceRoll.allDiceUsed()) {
        throw PksException{
            "PksGame::rollDice(): can't roll a new dice until all previous dice have been used."
        };
    }

    // Execute through a pointer for the polymorphic behaviour required by mocks in tests
    auto diceRoll = (&diceRoller)->rollNewPair();
    numConsecutiveDiceRolls++;

    // If the player doesn't have any piece at play, they need doubles before being allowed to use the dice
    bool canUseDiceRoll;
    if (diceRoll.isDoubles) {
        if (players.at(currentPlayer).anyPieceAtHome()) {
            // Implicitly use the dice roll to get our of Home/Jail
            players.at(currentPlayer).moveAllPiecesOutOfHome();
            canUseDiceRoll = false;
            nextPlayer();
        } else {
            canUseDiceRoll = true;
        }
    } else {
        // Can only use a non-doubles roll if there is at least 1 piece in play
        canUseDiceRoll = !players.at(currentPlayer).allPlayingPiecesAtHome();

        if (numConsecutiveDiceRolls == MAX_DICE_ROLLS) {
            nextPlayer();
        }
    }

    diceRoll.dicePair.first.alreadyUsed = !canUseDiceRoll;
    diceRoll.dicePair.second.alreadyUsed = !canUseDiceRoll;

    lastRollDiceResult = std::make_unique<PksRollDiceResult>(PksRollDiceResult{
        .diceRoll = diceRoll,
        .canUseDiceRoll = canUseDiceRoll,
        .canRollAgain = diceRoll.isDoubles && numConsecutiveDiceRolls != MAX_DICE_ROLLS,
    });
    return diceRoll.dicePair;
}

void PksGame::nextPlayer() {
    const int colorIndex = (static_cast<int>(currentPlayer) + 1) % NUM_PLAYERS;
    currentPlayer = static_cast<PksColor>(colorIndex);
    numConsecutiveDiceRolls = 0;
}

PksColor PksGame::useDice(const int diceValue, const int numPiece) {
    validateGameState("PksGame::useDice()", PksGameState::GameInCourse);

    if (numPiece < 0 || numPiece >= NUM_PIECES) {
        throw PksException{
            "PksGame::useDice(): Attempted to move invalid piece: " + std::to_string(numPiece)
        };
    }

    lastRollDiceResult->markDiceAsUsed(diceValue);

    moveCurrentPlayerPiece(numPiece, diceValue);

    if (lastRollDiceResult->diceRoll.allDiceUsed() && !lastRollDiceResult->canRollAgain) {
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
    };
}

void PksGame::moveCurrentPlayerPiece(int piece, int numSpots) {
    validateGameState("PksGame::movePiece()", PksGameState::GameInCourse);

    // New position in player local numbering
    int newPos = players.at(currentPlayer).movePiece(piece, numSpots);

    const auto spotType = getSpotType(newPos);
    if (spotType == PksSpotType::Goal && players.at(currentPlayer).allPiecesAtTarget()) {
        gameState = PksGameState::GameNotStarted;
    }

    // Check if this player's piece has fallen into an unsafe shared spot occupied by other players' pieces, and
    // capture them.
    if (spotType == PksSpotType::UnsafeShared) {
        for (auto &[otherColor, otherPlayer]: players) {
            // Ignore the current player, only interested in other players
            if (otherColor == currentPlayer) {
                continue;
            }

            int otherPlayersSpot = PksUtils::convertSpotNumber(currentPlayer, otherPlayer.getColor(), newPos);
            otherPlayer.capturePiecesAt(otherPlayersSpot);
        }
    }
}

/**
 * Gets the type of spot based on its number. Spots have numbers only from the perspective of players, there is no
 * global numbering.
 *
 * @param spot Spot identifier in player local numbering.
 * @return the type of spot.
 */
PksSpotType PksGame::getSpotType(int spot) {
    // Initial spot, treated as both Home and Jail
    if (spot == -1) { return PksSpotType::Home; }

    // Private stair, when the player gets here its pieces can't be eaten
    if (spot >= 63 && spot <= 70) {
        return PksSpotType::SafePrivate;
    }

    // Final spot, a piece can't move anymore after it has reached the target
    if (spot == 71) {
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

void PksGame::validateGameState(const std::string &methodName, const PksGameState expectedState) const {
    if (expectedState == gameState) {
        return;
    }

    switch (expectedState) {
        case PksGameState::GameNotStarted:
            throw PksException{methodName + ": Expected the game to not be started"};
        case PksGameState::GameInCourse:
            throw PksException{methodName + ": Expected the game to be in course"};
    }

    // Should never get here as the switch above needs to be exhaustive
    throw std::runtime_error{
        methodName + ": Unexpected game state: " + std::to_string(static_cast<int>(expectedState))
    };
}
