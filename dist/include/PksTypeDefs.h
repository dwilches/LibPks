#ifndef LIBPKS_PKSTYPEDEFS_H
#define LIBPKS_PKSTYPEDEFS_H

#include <map>
#include <vector>

enum class PksColor;

// Identifies each of the spots of the board. Can contain numbers between -1 (HOME_SPOT) and 71 (FINAL_TARGET_SPOT)
typedef int PksSpotIdx;

// Identifies each of the pieces of a player. Can contain numbers between 0 and 3 (NUM_PIECES - 1)
typedef int PksPieceIdx;

// Value returned by a dice. Can contain values 1 to 6.
typedef int PksDiceVal;

typedef std::pair<PksDiceVal, PksDiceVal> PksDicePair;

typedef std::map<PksColor, std::vector<int> > PksPiecesByPlayer;

#endif //LIBPKS_PKSTYPEDEFS_H
