#include <algorithm>
#include "PksPlayer.h"

#include "PksConstants.h"
#include "PksException.h"

PksPlayer::PksPlayer(int numPlayer, const PksColor &color)
    : numPlayer{numPlayer}, color{color}, pieces(NUM_PIECES, HOME_SPOT) {
}

int PksPlayer::getNumPlayer() const {
    return numPlayer;
}

const std::vector<int> &PksPlayer::getPieces() const {
    return pieces;
}

void PksPlayer::setPieces(const std::vector<int> &newPieces) {
    pieces = newPieces;
}

PksColor PksPlayer::getColor() const {
    return color;
}

int PksPlayer::movePiece(int piece, int numSpots) {
    if (pieces[piece] == FINAL_TARGET_SPOT) {
        throw PksException{
            "PksPlayer::movePiece(): Attempted to move a piece that is out of play: " + std::to_string(piece)
        };
    }

    const int newPos = std::min(pieces[piece] + numSpots, FINAL_TARGET_SPOT);
    pieces[piece] = newPos;
    return newPos;
}

bool PksPlayer::allPlayingPiecesAtHome() const {
    return std::ranges::all_of(pieces, [](const int piece) { return piece == HOME_SPOT; });
}

bool PksPlayer::allPiecesAtTarget() const {
    return std::ranges::all_of(pieces, [](const int piece) { return piece == FINAL_TARGET_SPOT; });
}

void PksPlayer::capturePiecesAt(const int numSpot) {
    for (auto &piece: pieces) {
        if (piece == numSpot) {
            piece = HOME_SPOT;
        }
    }
}

bool PksPlayer::anyPieceAtHome() {
    return std::ranges::any_of(pieces, [](const int piece) { return piece == HOME_SPOT; });
}

void PksPlayer::moveAllPiecesOutOfHome() {
    for (auto &piece: pieces) {
        if (piece == HOME_SPOT) {
            // Out of Home/Jail
            piece = 0;
        }
    }
}
