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
    explicit PksGameBoard(const std::vector<PksColor>& playerColors);

    // Returns a copy of the location of every piece of every player
    [[nodiscard]] PksPiecesByPlayer getPieces() const;

    // Allows setting arbitrary locations for every piece of every player.
    // Mostly useful for tests.
    void setPieces(const PksPiecesByPlayer &);

    [[nodiscard]] bool anyPieceAtSpot(PksColor, SPOT_IDX) const;

    [[nodiscard]] bool allPiecesAtTarget(PksColor) const;

    // Returns true only if the playing pieces (e.g. pieces not at the final position) are all at home.
    [[nodiscard]] bool allPlayingPiecesAtHome(PksColor) const;

    void moveAllPiecesOutOfHome(PksColor);

    // Moves all playing pieces (e.g. pieces not at the final position) back home.
    void moveAllPlayingPiecesHome(PksColor);

    // Moves a specific piece a number of spots. It validates the piece is not at home/target as those pieces can't be
    // moved.
    int movePiece(PksColor, PIECE_IDX, int numSpots);

    // Moves a group of pieces back home. It validates the pieces are in play.
    void movePiecesHome(PksColor, const std::set<PIECE_IDX> &);

    // Invoked when a player falls in this spot, so all pieces of other players at that spot need to go home.
    // Returns true if any piece was captured.
    bool movePiecesHomeIfAtSpot(PksColor, SPOT_IDX);

    //region Other methods that should be private but are exposed for tests

    // Returns the number of captured pieces and updates the given board in place
    static int movePiece(PksPiecesByPlayer &board, PksColor currentPlayer, PIECE_IDX pieceIdx, DICE_VAL diceVal);

    //endregion
};

#endif //LIBPKS_PKSGAMEBOARD_H
