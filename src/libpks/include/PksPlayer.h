#ifndef LIBPKS_PKSPLAYER_H
#define LIBPKS_PKSPLAYER_H

#include <vector>

class PksPlayer {
    // Position occupied by each piece.
    std::vector<int> pieces;

public:
    PksPlayer();

    explicit PksPlayer(const std::vector<int> &pieces);

    // Returns a copy of the piece positions
    [[nodiscard]] std::vector<int> getPieces() const;

    [[nodiscard]] bool anyPieceAtHome() const;

    [[nodiscard]] bool allPlayingPiecesAtHome() const;

    [[nodiscard]] bool allPiecesAtTarget() const;

    int movePiece(int piece, int numSpots);

    // Invoked when another player falls in this spot, so all of the current pieces need to go home
    void movePiecesHomeIfAtSpot(int numSpot);

    // Invoked when the player rolls doubles
    void moveAllPiecesOutOfHome();
};


#endif //LIBPKS_PKSPLAYER_H
