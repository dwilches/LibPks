
#ifndef LIBPKS_PKSCONSTANTS_H
#define LIBPKS_PKSCONSTANTS_H

// At the beginning of the game, all pieces are at the Home, and a player needs to roll doubles in order to get the
// pieces into play.
#define HOME_SPOT (-1)

// There are a total of 68 shared spots, which are divided in 4 "quadrants" each of 17 spots
#define TOTAL_SHARED_SPOTS 68

// Even though there are 68 shared spots, each Player can only reach 63 of them.
// For example, the spots right before the Yellow start spot cannot be reached by Yellow pieces, as they go up the
// Yellow stair once they've run around the board.
#define LAST_SHARED_SPOT 63

// After a piece reaches this spot, it's out of the game. Once all of a player's pieces reach the spot, the game ends.
#define FINAL_TARGET_SPOT 71

#define NUM_PLAYERS 4
#define NUM_PIECES 4

// Maximum number of double rolls allowed
#define MAX_DICE_ROLLS 3


#endif //LIBPKS_PKSCONSTANTS_H
