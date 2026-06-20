#ifndef LIBPKS_PKSSNITCHABLEPIECES_H
#define LIBPKS_PKSSNITCHABLEPIECES_H

#include <set>
#include <memory>

#include "PksGameBoard.h"
#include "PksMoves.h"
#include "PksTypeDefs.h"

class PksSnitcher {
    const PksColor currentPlayer;
    const PksDicePair dicePair;

    bool playerCanBeSnitchedNow;

    // There can be multiple optimal plays in a turn. An optimal play is a sequence of movements with the current
    // dice that captures the maximum number of pieces.
    PksDMoveSet optimalMoves;

    // Keep track of moves that the current player actually made. This is useful to know at which point the player
    // can be snitched.
    PksDMove actualMoves;

public:
    // Constructs an object with all the information of which pieces could be snitched if dice are not played in a
    // certain order or to capture a certain piece.
    PksSnitcher(const PksGameBoard &gameBoard,
                PksColor currentPlayer,
                const PksDicePair &dicePair);

    [[nodiscard]] PksColor getSnitchablePlayer() const;

    // When a player wastes a dice, this method returns the pieces were the dice could have been used to reach the
    // max number of captures. All pieces that didn't fulfill their part in the optimal captures become snitchable.
    // Special care is put for mutually exclusive optimal plays.
    [[nodiscard]] std::set<PIECE_IDX> getSnitchablePieces() const;

    // Get the list of possible optimal plays. There may be several optimal alternatives (i.e. sequence of movements
    // that yield the maximum number of captures).
    [[nodiscard]] PksDMoveSet getOptimalMoves() const;

    // When the current player makes a move, we need to report it to this class so we can recalculate the snitchable
    // pieces (in case the movement is not part of any optimal play).
    void reportDiceUsed(DICE_VAL, PIECE_IDX);

    // When a player snitches on another, we need to validate the snitch is valid before capturing the pieces.
    [[nodiscard]] bool isSnitchValid(const PksColor &snitchedPlayer,
                                     const std::set<PIECE_IDX> &snitchedPieces) const;

    //region Other methods that should be private but are exposed for tests

    [[nodiscard]] static PksDMoveSet
    getCapturingMoves(const PksPiecesByPlayer &piecesByPlayer,
                      const PksColor &currentPlayer,
                      const PksDicePair &dicePair);

    [[nodiscard]] static PksDMoveSet
    getCapturingMoves(const PksPiecesByPlayer &piecesByPlayer,
                      PksColor currentPlayer,
                      DICE_VAL firstDice,
                      DICE_VAL secondDice);

    [[nodiscard]] static std::vector<std::pair<PksSMove, PksPiecesByPlayer> >
    getPossibleMoves(const PksPiecesByPlayer &piecesByPlayer,
                     const PksColor &currentPlayer,
                     const DICE_VAL &diceValue);

    //endregion

private:
    static PksDMoveSet getOptimalCapturingMoves(const PksDMoveSet &capturingMoves);
};

#endif //LIBPKS_PKSSNITCHABLEPIECES_H
