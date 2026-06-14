#include <algorithm>
#include "PksPlayer.h"

#define GOAL_POSITION 71
#define HOME_POSITION -1

PksPlayer::PksPlayer(int numPlayer, const PksColor &color)
    : numPlayer{numPlayer}, color{color}, pieces(6, HOME_POSITION) {
}

int PksPlayer::getNumPlayer() const {
    return numPlayer;
}

const std::vector<int>& PksPlayer::getPieces() const {
    return pieces;
}

void PksPlayer::setPieces(const std::vector<int>& newPieces) {
    pieces = newPieces;
}

PksColor PksPlayer::getColor() const {
    return color;
}

int PksPlayer::movePiece(int piece, int numSpots) {
    const int newPos = std::min(pieces[piece] + numSpots, GOAL_POSITION);
    pieces[piece] = newPos;
    return newPos;
}

bool PksPlayer::allPlayingPiecesAtHome() const {
    return std::ranges::all_of(pieces, [](const int piece) { return piece == HOME_POSITION; });
}

bool PksPlayer::allPiecesAtTarget() const {
    return std::ranges::all_of(pieces, [](const int piece) { return piece == GOAL_POSITION; });
}

void PksPlayer::capturePiecesAt(const int numSpot) {
    for (auto &piece: pieces) {
        if (piece == numSpot) {
            piece = HOME_POSITION;
        }
    }
}

bool PksPlayer::anyPieceAtHome() {
    return std::ranges::any_of(pieces, [](const int piece) { return piece == HOME_POSITION; });
}

void PksPlayer::moveAllPiecesOutOfHome() {
    for (auto &piece: pieces) {
        if (piece == HOME_POSITION) {
            // Out of Home/Jail
            piece = 0;
        }
    }
}
