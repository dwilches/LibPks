#include "catch2/catch_test_macros.hpp"

#include "MockDiceRoller.h"
#include "PksConstants.h"
#include "PksGame.h"
#include "src/TestBoards.h"

TEST_CASE("Two optimal moves, move same piece but different dice") {
    const auto board = PksGameBoard({
        {PksColor::Yellow, {17 + 1, 17 + 2, HOME_SPOT, HOME_SPOT}},
        {PksColor::Red, {HOME_SPOT, HOME_SPOT, HOME_SPOT, 0}},
        {PksColor::Green, ALL_AT_HOME},
        {PksColor::Blue, ALL_AT_START},
    });

    // Red can capture Yellow pieces in spots 1 and 2 with these dice
    REQUIRE(PksSnitcher(board, PksColor::Red, {1, 2}).getOptimalMoves() == PksDMoveSet{
            {{3, 1, 1}},
            {{3, 2, 1}},
            });
}

TEST_CASE("One optimal moves, move same piece with both dice") {
    const auto board = PksGameBoard({
        {PksColor::Yellow, {17 + 1, 17 + 3, HOME_SPOT, HOME_SPOT}},
        {PksColor::Red, {HOME_SPOT, HOME_SPOT, HOME_SPOT, 0}},
        {PksColor::Green, ALL_AT_HOME},
        {PksColor::Blue, ALL_AT_START},
    });

    // Optimal needs using two dice in a specific order
    REQUIRE(PksSnitcher(board, PksColor::Red, {1, 2}).getOptimalMoves() == PksDMoveSet{
            {{3, 1, 1}, {3, 2, 1}},
            });

    // The order the dice were rolled in doesn't matter
    REQUIRE(PksSnitcher(board, PksColor::Red, {2, 1}).getOptimalMoves() == PksDMoveSet{
            {{3, 1, 1}, {3, 2, 1}},
            });
}

TEST_CASE("Two optimal moves, different pieces") {
    const auto board = PksGameBoard({
        {PksColor::Yellow, {17 + 1, 17 + 4, HOME_SPOT, HOME_SPOT}},
        {PksColor::Red, {0, 3, HOME_SPOT, HOME_SPOT}},
        {PksColor::Green, ALL_AT_HOME},
        {PksColor::Blue, ALL_AT_HOME},
    });

    // Move is duplicated, because they are using the dice in different order
    REQUIRE(PksSnitcher(board, PksColor::Red, {1, 1}).getOptimalMoves() == PksDMoveSet{
            {{0, 1, 1}, {1, 1, 1}},
            {{1, 1, 1}, {0, 1, 1}},
            });
}

TEST_CASE("Two optimal moves, need to choose which dice to use") {
    const auto board = PksGameBoard({
        {PksColor::Yellow, {17 + 1, 17 + 6, HOME_SPOT, HOME_SPOT}},
        {PksColor::Red, {0, HOME_SPOT, HOME_SPOT, HOME_SPOT}},
        {PksColor::Green, ALL_AT_HOME},
        {PksColor::Blue, ALL_AT_HOME},
    });

    REQUIRE(PksSnitcher(board, PksColor::Red, {1, 6}).getOptimalMoves() == PksDMoveSet{
            {{0, 1, 1} },
            {{0, 6, 1} },
            });
}

TEST_CASE("One optimal move, many play possibilities") {
    const auto board = PksGameBoard({
        {PksColor::Yellow, {17 + 1, 17 + 6, 17 + 10, HOME_SPOT}},
        {PksColor::Red, {0, 5, 9, HOME_SPOT}},
        {PksColor::Green, ALL_AT_HOME},
        {PksColor::Blue, ALL_AT_HOME},
    });

    // Move is duplicated, because they are using the dice in different order
    REQUIRE(PksSnitcher(board, PksColor::Red, {1, 6}).getOptimalMoves() == PksDMoveSet{
            {{0, 6, 1}, {2, 1, 1} },
            {{2, 1, 1}, {0, 6, 1} },
            });
}

TEST_CASE("Each dice allows capturing different pieces, but only 1 is optimal") {
    const auto board = PksGameBoard({
        {PksColor::Yellow, {17 + 2, 17 + 4, 17 + 4, 17 + 6}},
        {PksColor::Red, {0, 5, 9, 2}},
        {PksColor::Green, ALL_AT_HOME},
        {PksColor::Blue, ALL_AT_HOME},
    });

    REQUIRE(PksSnitcher(board, PksColor::Red, {2, 4}).getOptimalMoves() == PksDMoveSet{
            {{0, 4, 2}, {0, 2, 1} },
            });
}

TEST_CASE("Multiple options for optimal (1)") {
    const auto board = PksGameBoard({
        {PksColor::Yellow, {0, 17 + 1, 17 + 3, 17 + 4}},
        {PksColor::Red, {0, 1, 2, 0}},
        {PksColor::Green, {0, 0, 0, 0}},
        {PksColor::Blue, {0, 0, 0, 0}},
    });

    REQUIRE(PksSnitcher(board, PksColor::Red, {2, 4}).getOptimalMoves() == PksDMoveSet{
            {{0, 4, 1}, {1, 2, 1} },
            {{3, 4, 1}, {1, 2, 1} },
            // Same, different order
            { {1, 2, 1}, {0, 4, 1}},
            { {1, 2, 1}, {3, 4, 1}},
            });
}

TEST_CASE("Multiple options for optimal (2)") {
    const auto board = PksGameBoard({
        {PksColor::Yellow, {17 + 1, 17 + 3, 17 + 5, 17 + 6}},
        {PksColor::Red, {0, 5, HOME_SPOT, HOME_SPOT}},
        {PksColor::Green, {0, 0, 0, 0}},
        {PksColor::Blue, {0, 0, 0, 0}},
    });

    REQUIRE(PksSnitcher(board, PksColor::Red, {1, 2}).getOptimalMoves() == PksDMoveSet{
            {{0, 1, 1}, {0, 2, 1} },
            });
}
