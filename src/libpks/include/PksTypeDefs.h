#ifndef LIBPKS_PKSTYPEDEFS_H
#define LIBPKS_PKSTYPEDEFS_H

// Identifies each of the spots of the board. Can contain numbers between -1 (HOME_SPOT) and 71 (FINAL_TARGET_SPOT)
typedef int SPOT_IDX;

// Identifies each of the pieces of a player. Can contain numbers between 0 and 3 (NUM_PIECES - 1)
typedef int PIECE_IDX;

// Index of the first or second dice. Can contain values 1 or 2.
typedef int DICE_IDX;

// Value returned by a dice. Can contain values 1 to 6.
typedef int DICE_VAL;

typedef std::pair<DICE_VAL, DICE_VAL> PksDicePair;

#endif //LIBPKS_PKSTYPEDEFS_H
