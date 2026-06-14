#ifndef LIBPKS_PKSPLAYER_H
#define LIBPKS_PKSPLAYER_H

#include <vector>

#include "PksColor.h"

class PksPlayer {
    const int numPlayer;
    const PksColor color;

    // Position occupied by each piece.
    std::vector<int> pieces;

public:
    PksPlayer(int numPlayer, const PksColor &color);

    int getNumPlayer() const;

    const std::vector<int> &getPieces() const;
    void setPieces(const std::vector<int>& newPieces);

    PksColor getColor() const;

    int movePiece(int piece, int numSpots);

    bool allPlayingPiecesAtHome() const;

    bool allPiecesAtTarget() const;

    void capturePiecesAt(int numSpot);

    bool anyPieceAtHome();

    void moveAllPiecesOutOfHome();

};


#endif //LIBPKS_PKSPLAYER_H
