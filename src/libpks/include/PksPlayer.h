#ifndef LIBPKS_PKSPLAYER_H
#define LIBPKS_PKSPLAYER_H

#include <set>
#include <vector>

#include "PksTypeDefs.h"

/**
 * This class contains logic for moving a single player's pieces around the board.
 * This is a class private to LibPks, never exposed to users of the library.
 */
class PksPlayer {
    // Position occupied by each piece.
    std::vector<int> pieces;

public:
    // Initializes all pieces at the Home spot. Used for normal games.
    PksPlayer();

    // Allows overriding the initial spot used by each piece. Used by automated tests.
    explicit PksPlayer(const std::vector<int> &pieces);

    // Returns a copy of the piece positions
    [[nodiscard]] std::vector<int> getPieces() const;

    [[nodiscard]] bool anyPieceAtHome() const;

    [[nodiscard]] bool anyPiecesAtSpot(int numSpot) const;

    [[nodiscard]] bool allPlayingPiecesAtHome() const;

    [[nodiscard]] bool allPiecesAtTarget() const;

    // Move our own piece some spots ahead. It takes care of not going beyond the target spot.
    int movePiece(int piece, int numSpots);

    // Invoked when another player falls in this spot, so all the pieces of this player at that spot need to go home.
    // Returns true if any piece was captured.
    bool movePiecesHomeIfAtSpot(int numSpot);

    // Invoked when the player rolls doubles and there was at least 1 piece home
    void moveAllPiecesOutOfHome();

    // Invoked when the player rolls doubles 3 times in a row
    void moveAllPlayingPiecesHome();

    // Useful for when a specific piece is captured. For example, when a snitch succeeds.
    void movePiecesHome(const std::set<PIECE_IDX> &);
};

#endif //LIBPKS_PKSPLAYER_H
