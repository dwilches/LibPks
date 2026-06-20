#include <ranges>

#include "catch2/catch_test_macros.hpp"

#include "MockDiceRoller.h"
#include "PksConstants.h"
#include "PksSnitcher.h"
#include "src/TestBoards.h"

typedef std::set<PksSMove> PksSMoveSet;

PksSMoveSet getPossibleMoves(const PksGameBoard &board,
                             const PksColor currentPlayer,
                             const DICE_VAL diceValue) {
    auto movesWithBoards = PksSnitcher::getPossibleMoves(board, currentPlayer, diceValue);
    PksSMoveSet moves;
    for (auto &move: movesWithBoards | std::views::keys) {
        moves.insert(move);
    }
    return moves;
}

TEST_CASE("Can capture a single piece") {
    const auto board = PksGameBoard({
        {PksColor::Yellow, {17 + 1, 17 + 2, 17 + 3, 17 + 4}},
        {PksColor::Red, {10, 10, 10, START_SPOT}},
        {PksColor::Green, ALL_AT_HOME},
        {PksColor::Blue, ALL_AT_START},
    });

    REQUIRE(getPossibleMoves(board, PksColor::Red, 2) ==
            PksSMoveSet{
            {0, 2, 0},
            {1, 2, 0},
            {2, 2, 0},
            {3, 2, 1}, // only this one captures
            });
}

TEST_CASE("Can capture several pieces") {
    const auto board = PksGameBoard({
        {PksColor::Yellow, {17 + 1, 17 + 2, 17 + 4, 17 + 4}},
        {PksColor::Red, {10, 10, 10, START_SPOT}},
        {PksColor::Green, ALL_AT_HOME},
        {PksColor::Blue, ALL_AT_START},
    });

    REQUIRE(getPossibleMoves(board, PksColor::Red, 4) ==
            PksSMoveSet{
            {0, 4, 0},
            {1, 4, 0},
            {2, 4, 0},
            {3, 4, 2}, // only this one captures
            });
}

TEST_CASE("Several pieces can capture the same piece") {
    const auto board = PksGameBoard({
        {PksColor::Yellow, {17 + 1, 17 + 2, 17 + 4, 17 + 4}},
        {PksColor::Red, {10, START_SPOT, 10, START_SPOT}},
        {PksColor::Green, ALL_AT_HOME},
        {PksColor::Blue, ALL_AT_START},
    });

    REQUIRE(getPossibleMoves(board, PksColor::Red, 4) ==
            PksSMoveSet{
            {0, 4, 0},
            {1, 4, 2}, // this one captures
            {2, 4, 0},
            {3, 4, 2}, // this one captures
            });
}

TEST_CASE("Several pieces can capture different pieces") {
    const auto board = PksGameBoard({
        {PksColor::Yellow, {17 + 4, 17 + 15, 17 + 15, 17 + 15}},
        {PksColor::Red, {11, 11, START_SPOT, 12}},
        {PksColor::Green, ALL_AT_HOME},
        {PksColor::Blue, ALL_AT_HOME},
    });

    REQUIRE(getPossibleMoves(board, PksColor::Red, 4) ==
            PksSMoveSet{
            {0, 4, 3},
            {1, 4, 3},
            {2, 4, 1},
            {3, 4, 0},
            });
}

TEST_CASE("Can't capture from home nor the final stair") {
    const auto board = PksGameBoard({
        {PksColor::Yellow, ALL_AT_START},
        {PksColor::Red, {HOME_SPOT, START_SPOT, LAST_SHARED_SPOT, FINAL_TARGET_SPOT}},
        {PksColor::Green, ALL_AT_HOME},
        {PksColor::Blue, ALL_AT_START},
    });

    REQUIRE(getPossibleMoves(board, PksColor::Red, 2) ==
            PksSMoveSet{
            {1, 2, 0}, // Only the piece at start can move
            });
}

TEST_CASE("Can capture pieces from different players") {
    const auto board = PksGameBoard({
        {PksColor::Yellow, {34 + 1, HOME_SPOT, HOME_SPOT, HOME_SPOT}},
        {PksColor::Red, {17 + 1, HOME_SPOT, HOME_SPOT, HOME_SPOT}},
        {PksColor::Green, {0, FINAL_TARGET_SPOT, FINAL_TARGET_SPOT, FINAL_TARGET_SPOT}},
        {PksColor::Blue, ALL_AT_HOME},
    });

    REQUIRE(getPossibleMoves(board, PksColor::Green, 1) ==
            PksSMoveSet{
            {0, 1, 2},
            });
}
