#ifndef LIBPKS_PKSGAMEBOARD_H
#define LIBPKS_PKSGAMEBOARD_H

#include <set>

#include "PksTypeDefs.h"

/**
 * This class holds the location of every piece of the game.
 * It also contains the logic for moving pieces around.
 */
class PksGameBoard {
    // All game pieces indexed by player.
    PksPiecesByPlayer boardPieces;

public:
    explicit PksGameBoard(const std::vector<PksColor> &playerColors);

    // Mostly useful for tests.
    explicit PksGameBoard(const PksPiecesByPlayer &);

    // Returns a copy of the location of every piece of every player
    [[nodiscard]] PksPiecesByPlayer getPieces() const;

    [[nodiscard]] PksSpotIdx getSpotForPiece(PksColor, PksPieceIdx) const;

    // Allows setting arbitrary locations for every piece of every player.
    // Mostly useful for tests.
    void setPieces(const PksPiecesByPlayer &);

    [[nodiscard]] bool anyPieceAtSpot(PksColor, PksSpotIdx) const;

    [[nodiscard]] bool allPiecesAtTarget(PksColor) const;

    // Returns true only if the playing pieces (e.g. pieces not at the final position) are all at home.
    [[nodiscard]] bool allPlayingPiecesAtHome(PksColor) const;

    void moveAllPiecesOutOfHome(PksColor);

    // Moves all playing pieces (e.g. pieces not at the final position) back home.
    void moveAllPlayingPiecesHome(PksColor);

    // Moves a piece a number of spots. It validates the piece is not at home/target as those pieces can't be moved.
    // Foreign pieces in the target spot are captured.
    // Returns the number of captured pieces.
    int movePiece(PksColor, PksPieceIdx, int numSpots);

    // Moves a group of pieces back home. It validates the pieces are in play.
    void movePiecesHome(PksColor, const std::set<PksPieceIdx> &);

private:
    // Invoked when a player falls in this spot, so all pieces of other players at that spot need to go home.
    // Returns the number of pieces captured.
    int movePiecesHomeIfAtSpot(PksColor, PksSpotIdx);
};

#endif //LIBPKS_PKSGAMEBOARD_H
