#ifndef PKSLIB_LIBRARY_H
#define PKSLIB_LIBRARY_H

#include <memory>

#include "PksDiceRoller.h"
#include "PksPlayer.h"
#include "PksDiceResult.h"
#include "PksBoardState.h"
#include "PksGameState.h"
#include "PksSpotType.h"

/**
 * Every Parqués piece can be in one of 73 different spots. To simplify the implementation, I chose these numbers for
 * the abstractions:
 *   - Spot -1 is the home/jail
 *   - Spot 0 is the start position, which the player reaches only after rolling doubles
 *   - Spots 0 through 63 are shared positions, in which other players can have their pieces too. Some are marked as
 *     "Safe" and others are "Unsafe". Pieces in unsafe spots can be captured if another coloured piece arrives.
 *   - Spot 64 through 71 are of private use of each player (each player has its own group of spots 64-71, which makes
 *     implementation of this game interesting).
 *   - Spot 71 is the goal. After reaching this spot, a piece can't be moved again. The game ends once all of a player's
 *     pieces reach the goal.
 *
 * The spots have different numbers depending on the player, this is what makes the implementation interesting and
 * difficult. For example, Player 1's (Yellow) spot 0 is the same as Player 2's spot 51, Player 3's spot 34 and
 * Player 4's spot 17. This means if Player 1 has a piece in spot 5, and once of Player 2's pieces arrives at its
 * position 55, then Player 1's piece is captured and sent to jail. This is more clearly seen in the README images.
 */
class PksGame {
    std::map<PksColor, PksPlayer> players;
    PksDiceRoller &diceRoller;
    PksGameState gameState;
    PksColor currentPlayer;
    int numConsecutiveDiceRolls;
    std::unique_ptr<PksDiceResult> lastRollDiceResult;

    void validateGameInCourse(const std::string &methodName) const;

    void moveCurrentPlayerPiece(int piece, int numSpots);

    // Moves all pieces at this spot home, except the pieces of the current player
    void moveHomeAllPiecesAtSpot(int spot);


    void nextPlayer();

public:
    // Used for testing. Allows mocking the Dice Roller to give the dice values the tests expect.
    explicit PksGame(PksDiceRoller &diceRoller);

    PksBoardState start();

    // Useful for tests
    PksBoardState start(const PksBoardState &boardState);

    void stop();

    PksDiceResult rollDice();

    PksBoardState useDice(int diceValue, int numPiece);

    [[nodiscard]] PksBoardState getCurrentBoardState() const;
};

#endif // PKSLIB_LIBRARY_H
