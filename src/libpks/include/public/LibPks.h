/*
 * Author: Daniel Wilches
 * Released under: GNU GENERAL PUBLIC LICENSE v3
 * Source code: https://github.com/dwilches/LibPks
 */

#ifndef LIBPKS_H
#define LIBPKS_H


#include <map>
#include <memory>
#include <set>
#include <vector>

enum class PksColor: int {
    Yellow, Red, Green, Blue
};

enum class PksGameState {
    GameNotStarted, GameInCourse, GameOver
};

struct PksGameSnapshot;
class PksDiceResult;

// Identifies each of the pieces of a player. Can contain numbers between 0 and 3 (NUM_PIECES - 1)
typedef int PIECE_IDX;

// Value returned by a dice. Can contain values 1 to 6.
typedef int DICE_VAL;

typedef std::pair<DICE_VAL, DICE_VAL> PksDicePair;
typedef std::map<PksColor, std::vector<int> > PksPiecesByPlayer;

class PksGame {

public:
    PksGame();

    // Invoked to make an instance of PksGame ready for playing
    PksGameSnapshot start();

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

struct PksGameSnapshot {
    PksPiecesByPlayer piecesByPlayer;
    PksColor currentPlayer;
    PksGameState gameState;
    std::set<PIECE_IDX> snitchablePieces;
};

class PksDiceResult {
public:
    [[nodiscard]] PksDicePair getDice() const;

    [[nodiscard]] bool allDiceUsed() const;
};

#endif // LIBPKS_H
