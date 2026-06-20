#ifndef LIBPKS_PKSUTILS_H
#define LIBPKS_PKSUTILS_H

#include "PksColor.h"
#include "PksSpotType.h"
#include "PksTypeDefs.h"

// These utils are meant to be private to LibPks, they should not be exposed to users of the library.
namespace PksUtils {
    // Spot are numbered according to the user that is using them, so Spot 0 for Red is not Spot 0 for Yellow, instead,
    // Spot 0 for Red is Spot 17 for Yellow.
    // This utility function makes conversions between spot numbers easier, and simplifies the overall implementation.
    int convertSpotNumber(PksColor from, PksColor to, PksSpotIdx spot);

    // Gets the type of spot to determine if it's a safe/unsafe/shared spot.
    // Spots have numbers only from the perspective of players, there is no global numbering.
    PksSpotType getSpotType(PksSpotIdx spot);
};

#endif //LIBPKS_PKSUTILS_H
