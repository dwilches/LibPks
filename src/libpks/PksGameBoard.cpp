#include "include/PksGameBoard.h"

#include <algorithm>
#include <ranges>

#include "PksConstants.h"
#include "PksException.h"
#include "PksUtils.h"

PksGameBoard::PksGameBoard(const std::vector<PksColor> &playerColors) {
    for (const auto color: playerColors) {
        boardPieces[color] = std::vector(NUM_PIECES, HOME_SPOT);
    }
}

void PksGameBoard::clear() {
    for (const auto color: boardPieces | std::views::keys) {
        boardPieces[color] = std::vector(NUM_PIECES, HOME_SPOT);
    }
}

PksPiecesByPlayer PksGameBoard::getPieces() const {
    return boardPieces;
}

void PksGameBoard::setPieces(const PksPiecesByPlayer &sourcePieces) {
    for (const auto color: boardPieces | std::views::keys) {
        boardPieces[color] = sourcePieces.at(color);
    }
}

bool PksGameBoard::anyPieceAtSpot(const PksColor color, const SPOT_IDX spotIdx) const {
    return std::ranges::any_of(boardPieces.at(color),
                               [spotIdx](const int piece) { return piece == spotIdx; });
}

bool PksGameBoard::allPiecesAtTarget(const PksColor color) const {
    return std::ranges::all_of(boardPieces.at(color),
                               [](const int piece) { return piece == FINAL_TARGET_SPOT; });
}

bool PksGameBoard::allPlayingPiecesAtHome(const PksColor color) const {
    return std::ranges::all_of(boardPieces.at(color),
                               [](const int piece) { return piece == HOME_SPOT || piece == FINAL_TARGET_SPOT; });
}

void PksGameBoard::moveAllPiecesOutOfHome(const PksColor color) {
    for (auto &piece: boardPieces.at(color)) {
        if (piece == HOME_SPOT) {
            piece = START_SPOT;
        }
    }
}

void PksGameBoard::moveAllPlayingPiecesHome(const PksColor color) {
    for (auto &piece: boardPieces.at(color)) {
        // Don't move pieces that are already out of play
        if (piece != FINAL_TARGET_SPOT) {
            piece = HOME_SPOT;
        }
    }
}

int PksGameBoard::movePiece(const PksColor color, const PIECE_IDX pieceIdx, const int numSpots) {
    auto &pieces = boardPieces.at(color);
    if (pieces[pieceIdx] == FINAL_TARGET_SPOT || pieces[pieceIdx] == HOME_SPOT) {
        throw PksException{
            "PksGameBoard::movePiece(): Attempted to move a piece that is out of play: " + std::to_string(pieceIdx)
        };
    }

    const int newSpotIdx = std::min(pieces[pieceIdx] + numSpots, FINAL_TARGET_SPOT);
    pieces[pieceIdx] = newSpotIdx;
    return newSpotIdx;
}

void PksGameBoard::movePiecesHome(const PksColor color, const std::set<PIECE_IDX> &targetPiecesIdx) {
    auto &pieces = boardPieces.at(color);
    for (const auto &pieceIdx: targetPiecesIdx) {
        if (pieces[pieceIdx] == FINAL_TARGET_SPOT) {
            throw PksException{
                "PksGameBoard::movePiece(): Attempted to move a piece that is out of play: " + std::to_string(pieceIdx)
            };
        }
        pieces[pieceIdx] = HOME_SPOT;
    }
}

bool PksGameBoard::movePiecesHomeIfAtSpot(const PksColor color, const SPOT_IDX spotIdx) {
    bool anyPieceCaptured = false;
    for (auto &piece: boardPieces.at(color)) {
        if (piece == spotIdx) {
            piece = HOME_SPOT;
            anyPieceCaptured = true;
        }
    }
    return anyPieceCaptured;
}

// This is a static method that acts on a given board, updating it in-place.
int PksGameBoard::movePiece(PksPiecesByPlayer &board,
                            const PksColor currentPlayer,
                            const PIECE_IDX pieceIdx,
                            const DICE_VAL diceVal) {
    auto &playerPieces = board.at(currentPlayer);
    if (playerPieces[pieceIdx] == FINAL_TARGET_SPOT || playerPieces[pieceIdx] == HOME_SPOT) {
        throw PksException{
            "PksGameBoard::movePiece(): Attempted to move a piece that is out of play: " + std::to_string(pieceIdx)
        };
    }

    // Move the player's piece to the target spot
    const SPOT_IDX targetSpot = std::min(playerPieces[pieceIdx] + diceVal, FINAL_TARGET_SPOT);
    playerPieces[pieceIdx] = targetSpot;

    // If the spot is not shared unsafe, there is nothign we can capture
    if (PksUtils::getSpotType(targetSpot) != PksSpotType::UnsafeShared) {
        return 0;
    }

    // Capture any pieces that are at the target spot
    int numCaptured = 0;
    for (auto &[otherPlayerColor, otherPlayerPieces]: board) {
        if (otherPlayerColor == currentPlayer) {
            continue;
        }

        const auto &otherSpot = PksUtils::convertSpotNumber(currentPlayer, otherPlayerColor, targetSpot);

        // If the other player has any piece at the target spot, move them back home
        for (auto &otherPlayerPiece: otherPlayerPieces) {
            if (otherPlayerPiece == otherSpot) {
                otherPlayerPiece = HOME_SPOT;
                numCaptured++;
            }
        }
    }
    return numCaptured;
}
