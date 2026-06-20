#ifndef LIBPKS_PKSMOVES_H
#define LIBPKS_PKSMOVES_H

#include <iosfwd>
#include <set>
#include <vector>

#include "PksTypeDefs.h"

// A single piece that is moved by a dice value, and how many foreign pieces it captured.
struct PksSMove {
    PIECE_IDX pieceIdx;
    DICE_VAL diceValue;
    int numCaptured;

    // Allows this struct to be used with sets
    auto operator<=>(const PksSMove &other) const = default;
};

// A sequence of either 1 PksSMove, or 2 PksSMoves. Order matters.
typedef std::vector<PksSMove> PksDMove;

// Collection of alternative DMoves a user can do.
typedef std::set<PksDMove> PksDMoveSet;

// Useful for debugging
std::ostream &operator<<(std::ostream &os, const PksSMove &move);

#endif //LIBPKS_PKSMOVES_H
