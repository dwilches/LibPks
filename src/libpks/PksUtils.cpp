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

PksSpotType PksUtils::getSpotType(const int spot) {
    // Initial spot, treated as both Home and Jail
    if (spot == HOME_SPOT) { return PksSpotType::Home; }

    // Private stair, when the player gets here its pieces can't be eaten
    if (spot >= LAST_SHARED_SPOT && spot < FINAL_TARGET_SPOT) {
        return PksSpotType::SafePrivate;
    }

    // Final spot, a piece can't move anymore after it has reached the target
    if (spot == FINAL_TARGET_SPOT) {
        return PksSpotType::Goal;
    }

    // Shared spots
    // Every 18 spots the types repeat, so simplify the spot number taking advantage of it being 0-indexed
    const int simplifiedSpot = spot % 17;
    if (simplifiedSpot == 0 || simplifiedSpot == 7 || simplifiedSpot == 12) {
        return PksSpotType::SafeShared;
    }

    return PksSpotType::UnsafeShared;
}

bool PksUtils::isValidAndUnsafeShared(const int possibleSpot) {
    return possibleSpot >= START_SPOT &&
           possibleSpot <= LAST_SHARED_SPOT &&
           getSpotType(possibleSpot) == PksSpotType::UnsafeShared;
}
