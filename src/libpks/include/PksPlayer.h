#ifndef LIBPKS_PKSPLAYER_H
#define LIBPKS_PKSPLAYER_H

#include <vector>

#include "PksColor.h"

class PksPlayer {
    // Position occupied by each piece.
    std::vector<int> pieces;

public:
    PksPlayer();

    explicit PksPlayer(const std::vector<int> &pieces);

    const std::vector<int> &getPieces() const;

    int movePiece(int piece, int numSpots);

    bool allPlayingPiecesAtHome() const;

    bool allPiecesAtTarget() const;

    void capturePiecesAt(int numSpot);

    bool anyPieceAtHome();

    void moveAllPiecesOutOfHome();
};


#endif //LIBPKS_PKSPLAYER_H
