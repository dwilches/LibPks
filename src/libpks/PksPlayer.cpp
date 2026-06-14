#include <algorithm>
#include "PksPlayer.h"

#include "PksConstants.h"
#include "PksException.h"

PksPlayer::PksPlayer() : PksPlayer{std::vector(NUM_PIECES, HOME_SPOT)} {
}

PksPlayer::PksPlayer(const std::vector<int> &pieces) : pieces{pieces} {
}

std::vector<int> PksPlayer::getPieces() const {
    return pieces;
}

bool PksPlayer::allPlayingPiecesAtHome() const {
    return std::ranges::all_of(pieces, [](const int piece) { return piece == HOME_SPOT; });
}

bool PksPlayer::anyPieceAtHome() const {
    return std::ranges::any_of(pieces, [](const int piece) { return piece == HOME_SPOT; });
}

bool PksPlayer::allPiecesAtTarget() const {
    return std::ranges::all_of(pieces, [](const int piece) { return piece == FINAL_TARGET_SPOT; });
}

int PksPlayer::movePiece(const int piece, const int numSpots) {
    if (pieces[piece] == FINAL_TARGET_SPOT) {
        throw PksException{
            "PksPlayer::movePiece(): Attempted to move a piece that is out of play: " + std::to_string(piece)
        };
    }

    const int newPos = std::min(pieces[piece] + numSpots, FINAL_TARGET_SPOT);
    pieces[piece] = newPos;
    return newPos;
}

void PksPlayer::movePiecesHomeIfAtSpot(const int numSpot) {
    for (auto &piece: pieces) {
        if (piece == numSpot) {
            piece = HOME_SPOT;
        }
    }
}

void PksPlayer::moveAllPiecesOutOfHome() {
    for (auto &piece: pieces) {
        if (piece == HOME_SPOT) {
            piece = START_SPOT;
        }
    }
}
