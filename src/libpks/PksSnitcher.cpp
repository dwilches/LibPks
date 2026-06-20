#include "PksSnitcher.h"

#include <algorithm>
#include <numeric>
#include <ranges>

#include "PksConstants.h"
#include "include/PksGameBoard.h"
#include "PksUtils.h"

PksSnitcher::PksSnitcher(const PksPiecesByPlayer &piecesByPlayer,
                         const PksColor currentPlayer,
                         const PksDicePair &dicePair)
    : piecesByPlayer{piecesByPlayer},
      currentPlayer{currentPlayer},
      dicePair{dicePair},
      playerCanBeSnitchedNow{false} {
    // Gets every possible move where at least 1 capture is made
    auto capturingMoves = getCapturingMoves(piecesByPlayer, currentPlayer, dicePair);
    if (capturingMoves.empty()) {
        maxBothDiceCanCapture = 0;
        return;
    }

    // Find the maximum number of pieces that can be captured, all sequences that can capture that many pieces are the
    // optimal plays.
    PksDMoveSet possibleOptimalMoves;
    int maxTotalCaptured = 0;
    for (const auto &sequence: capturingMoves) {
        int totalCaptured = 0;
        for (const auto &move: sequence) {
            totalCaptured += move.numCaptured;
        }

        // Every sequence of moves that yield the maximum is an optimal move
        if (totalCaptured == maxTotalCaptured) {
            possibleOptimalMoves.insert(sequence);
            continue;
        }

        // Found a new maximum, the old optimal moves are no longer optimal
        if (totalCaptured > maxTotalCaptured) {
            possibleOptimalMoves.clear();
            maxTotalCaptured = totalCaptured;
            possibleOptimalMoves.insert(sequence);
        }
    }
    optimalMoves = std::move(possibleOptimalMoves);
    maxBothDiceCanCapture = maxTotalCaptured;
}


// Returns possible moves where at least one piece is captured
PksDMoveSet
PksSnitcher::getCapturingMoves(const PksPiecesByPlayer &piecesByPlayer,
                               const PksColor &currentPlayer,
                               const PksDicePair &dicePair) {
    auto movesOrder1 = getCapturingMoves(piecesByPlayer, currentPlayer, dicePair.first, dicePair.second);
    auto movesOrder2 = getCapturingMoves(piecesByPlayer, currentPlayer, dicePair.second, dicePair.first);
    movesOrder1.merge(movesOrder2);
    return movesOrder1;
}

// Returns possible moves where at least one piece is captured. Dice need to be used in this order.
PksDMoveSet
PksSnitcher::getCapturingMoves(const PksPiecesByPlayer &piecesByPlayer,
                               const PksColor currentPlayer,
                               const DICE_VAL firstDice,
                               const DICE_VAL secondDice) {
    PksDMoveSet capturingMoves;

    // Simulate moving the first dice first
    const auto movesDice1 = getPossibleMoves(piecesByPlayer, currentPlayer, firstDice);

    // Then simulate moving the second dice for each of those first moves
    for (const auto &[move1, newBoard]: movesDice1) {
        const auto movesDice2 = getPossibleMoves(newBoard, currentPlayer, secondDice);

        bool anyCaptureWithDice2 = false;
        for (const auto &move2: movesDice2 | std::views::keys) {
            // If the second move didn't capture any piece, then ignore it, it's not relevant for the optimal move
            // calculation
            if (move2.numCaptured == 0) {
                continue;
            }
            anyCaptureWithDice2 = true;

            // If both moves captured pieces, then this is a relevant 2-step move
            if (move1.numCaptured > 0) {
                capturingMoves.insert(std::vector{move1, move2});
            } else {
                // If the first move didn't capture any piece, but the second move did, then:
                // - if the same piece was moved both times, this is a relevant 2-step move
                // - if different pieces were moved, only the second move is relevant
                if (move1.pieceIdx == move2.pieceIdx) {
                    capturingMoves.insert(std::vector{move1, move2});
                } else {
                    capturingMoves.insert(std::vector{move2});
                }
            }
        }

        // If the first move captured something, but none of the second moves captured anything, then moving the first
        // dice is the relevant move
        if (move1.numCaptured > 0 && !anyCaptureWithDice2) {
            capturingMoves.insert(std::vector{move1});
        }
    }

    return capturingMoves;
}

// Returns possible move even where no pieces are captured.
std::vector<std::pair<PksSMove, PksPiecesByPlayer> >
PksSnitcher::getPossibleMoves(const PksPiecesByPlayer &piecesByPlayer,
                              const PksColor &currentPlayer,
                              const DICE_VAL &diceValue) {
    const auto &thisPlayerPieces = piecesByPlayer.at(currentPlayer);

    std::vector<std::pair<PksSMove, PksPiecesByPlayer> > possibleMoves;

    // Check all pieces this dice can be used on, and calculate the resulting board.
    for (int thisPieceIdx = 0; thisPieceIdx < NUM_PIECES; thisPieceIdx++) {
        const int thisPieceSpot = thisPlayerPieces[thisPieceIdx];
        // Can't move pieces that are home. Also, if the piece is in the final stair, it can't capture anything anymore.
        if (thisPieceSpot == HOME_SPOT || thisPieceSpot >= LAST_SHARED_SPOT) {
            continue;
        }

        // Duplicate board so we can update it
        auto newBoard = piecesByPlayer;
        const int numCaptured = PksGameBoard::movePiece(newBoard, currentPlayer, thisPieceIdx, diceValue);
        const PksSMove move = {
            .pieceIdx = thisPieceIdx,
            .diceValue = diceValue,
            .numCaptured = numCaptured
        };
        possibleMoves.emplace_back(move, newBoard);
    }

    return possibleMoves;
}

PksColor PksSnitcher::getSnitchablePlayer() const {
    return currentPlayer;
}

int PksSnitcher::getMaxCanBeCaptured() const {
    return maxBothDiceCanCapture;
}

PksDMoveSet PksSnitcher::getOptimalMoves() const {
    return optimalMoves;
}

std::set<PIECE_IDX> PksSnitcher::getSnitchablePieces() const {
    std::set<PIECE_IDX> snitchedPieces;
    for (const auto &moves: optimalMoves) {
        for (const auto &move: moves) {
            snitchedPieces.insert(move.pieceIdx);
        }
    }
    return snitchedPieces;
}

void PksSnitcher::reportDiceUsed(const DICE_VAL diceValue, const PIECE_IDX pieceIdx) {
    actualMoves.emplace_back(pieceIdx, diceValue);

    // If the player has already used the 2 dice in this turn, then the complete move should be contained in
    // the list of optimal moves. Otherwise, the player can be snitched.
    if (actualMoves.size() == 2) {
        playerCanBeSnitchedNow = !optimalMoves.contains(actualMoves);
    } else {
        // If this is just the first dice, then check if this move could be the first move of any optimal move.
        // Otherwise, the player can be snitched.
        for (const auto &moveSequence: optimalMoves) {
            if (moveSequence[0] == actualMoves[0]) {
                // Nothing more to do, the player cannot be snitched
                return;
            }
        }
        playerCanBeSnitchedNow = true;
    }
}

bool PksSnitcher::isSnitchValid(const PksColor &snitchedPlayer,
                                const std::set<PIECE_IDX> &snitchedPieces) const {
    return snitchedPlayer == currentPlayer &&
           std::ranges::includes(getSnitchablePieces(), snitchedPieces);
}
