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

PksGameBoard::PksGameBoard(const PksPiecesByPlayer &playerColors) {
    boardPieces = playerColors;
}

PksPiecesByPlayer PksGameBoard::getPieces() const {
    return boardPieces;
}

SPOT_IDX PksGameBoard::getSpotForPiece(const PksColor color, const PIECE_IDX pieceIdx) const {
    return boardPieces.at(color)[pieceIdx];
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

    // Capture all pieces that were walking by our Home
    for (const auto &otherPlayerColor: boardPieces | std::views::keys) {
        // Ignore current player
        if (otherPlayerColor == color) {
            continue;
        }
        const auto &otherSpotIdx = PksUtils::convertSpotNumber(color, otherPlayerColor, START_SPOT);
        movePiecesHomeIfAtSpot(otherPlayerColor, otherSpotIdx);
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

    // Move the player's piece to the target spot
    const auto newSpotIdx = std::min(pieces[pieceIdx] + numSpots, FINAL_TARGET_SPOT);
    pieces[pieceIdx] = newSpotIdx;

    // If the spot is not shared unsafe, there is nothing we can capture
    if (PksUtils::getSpotType(newSpotIdx) != PksSpotType::UnsafeShared) {
        return 0;
    }

    // Capture any pieces that are at the target spot
    int numCaptured = 0;
    for (const auto &otherPlayerColor: boardPieces | std::views::keys) {
        // Ignore current player
        if (otherPlayerColor == color) {
            continue;
        }

        const auto &otherSpotIdx = PksUtils::convertSpotNumber(color, otherPlayerColor, newSpotIdx);
        numCaptured += movePiecesHomeIfAtSpot(otherPlayerColor, otherSpotIdx);
    }
    return numCaptured;
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

int PksGameBoard::movePiecesHomeIfAtSpot(const PksColor color, const SPOT_IDX spotIdx) {
    int numCaptured = 0;
    for (auto &piece: boardPieces.at(color)) {
        if (piece == spotIdx) {
            piece = HOME_SPOT;
            numCaptured++;
        }
    }
    return numCaptured;
}
