#ifndef LIBPKS_PKSGAMEBOARD_H
#define LIBPKS_PKSGAMEBOARD_H

#include "PksGameSnapshot.h"

class PksGameBoard {
public:
    // Returns the number of captured pieces and updates the board in place
    static int movePiece(PksPiecesByPlayer &board, PksColor currentPlayer, PIECE_IDX pieceIdx, DICE_VAL diceVal);
};

#endif //LIBPKS_PKSGAMEBOARD_H
