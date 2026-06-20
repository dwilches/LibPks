#ifndef PKSLIB_LIBRARY_H
#define PKSLIB_LIBRARY_H

#include <memory>
#include <set>

#include "PksDiceRoller.h"
#include "PksDiceResult.h"
#include "PksGameBoard.h"
#include "PksGameSnapshot.h"
#include "PksSnitcher.h"

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
    PksGameBoard gameBoard;
    PksDiceRoller &diceRoller;
    PksGameState gameState = PksGameState::GameNotStarted;
    int numConsecutiveDiceRolls = 0;

    // Naked pointer as it won't be our responsibility to free it up
    const PksColor* currentPlayer = nullptr;

    // When a player loses the opportunity to capture a piece, another player can snitch on them.
    std::unique_ptr<PksSnitcher> snitchablePieces = nullptr;

    // Keeps track of the last dice values and whether they have already been used
    std::unique_ptr<PksDiceResult> lastRollDiceResult = nullptr;

    // Validates the current game state before invoking a method taht has prerrequisites.
    void validateGameInCourse(const std::string &methodName) const;

    // Returns true if any piece was captured.
    bool moveCurrentPlayerPiece(int piece, int numSpots);

    // Moves all pieces at this spot home, except the pieces of the current player.
    // Returns true if any piece was captured.
    bool moveHomeAllPiecesAtSpot(int spot);

    // Ends a player's turn and starts the next one
    void nextPlayer();

public:
    // Used for testing. Allows mocking the Dice Roller to give the dice values the tests expect.
    explicit PksGame(PksDiceRoller &diceRoller);

    // Invoked to make an instance of PksGame ready for playing
    PksGameSnapshot start();

    // Useful for tests
    PksGameSnapshot start(const PksGameSnapshot& gameSnapshot);

    // Invoked to roll 2 random dice
    PksDiceResult rollDice();

    // Moves a piece of the current player a certain number of spots
    PksGameSnapshot useDice(int diceValue, int numPiece);

    // Capturing a piece is mandatory: when a player misses the opportunity to capture a piece, then their own piece
    // is punished by sending it home (but only if another player catches the mistake).
    // Returns `true` if the snitch was valid.
    bool snitchOnPlayer(const PksColor& snitched, const std::set<PIECE_IDX>& pieces);

    // Returns the current location of every piece of the game, as well as whether the game has finished and who the
    // current player is.
    [[nodiscard]] PksGameSnapshot getGameSnapshot() const;
};

#endif // PKSLIB_LIBRARY_H
