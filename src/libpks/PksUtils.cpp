#include <sstream>

#include "PksUtils.h"
#include "PksConstants.h"
#include "PksException.h"

int PksUtils::convertSpotNumber(const PksColor &from, const PksColor &to, const int spot) {
    //region Validations
    if (spot < 0 || spot > LAST_SHARED_SPOT) {
        throw PksException{
            "PksUtils::convertSpotNumber(): Attempted to convert invalid spot number: " + std::to_string(spot)
        };
    }
    if (from == to) {
        std::ostringstream oss;
        oss << "PksUtils::convertSpotNumber(): Attempted to convert a spot number from a player to itself: "
                << from << " spot number: " << spot;
        throw PksException{oss.str()};
    }
    //endregion

    int playerDistance = static_cast<int>(from) - static_cast<int>(to);

    // Using positive numbers makes the following calculation much easier
    if (playerDistance < 0) {
        playerDistance += NUM_PLAYERS;
    }

    // Every shared spot of a color is either 17, 34 or 51, ahead or behind, every other color.
    return (spot + (playerDistance * 17)) % TOTAL_SHARED_SPOTS;
}
