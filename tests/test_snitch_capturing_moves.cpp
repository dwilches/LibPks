#include "catch2/catch_test_macros.hpp"

#include "MockDiceRoller.h"
#include "PksConstants.h"
#include "PksGame.h"
#include "src/TestBoards.h"

TEST_CASE("Capture 1 different piece per dice") {
    const auto board = PksPiecesByPlayer{
        {PksColor::Yellow, {17 + 1, 17 + 2, HOME_SPOT, HOME_SPOT}},
        {PksColor::Red, {HOME_SPOT, HOME_SPOT, HOME_SPOT, 0}},
        {PksColor::Green, ALL_AT_HOME},
        {PksColor::Blue, ALL_AT_START},
    };

    // Red can capture Yellow pieces in spots 1 and 2 with these dice
    REQUIRE(PksSnitchablePieces::getCapturingMoves(board, PksColor::Red, {1, 2}) == CapturingMoveSequences{
            {{3, 1, 1}},
            {{3, 2, 1}},
            });
}

TEST_CASE("Capture needs both dice") {
    const auto board = PksPiecesByPlayer{
            {PksColor::Yellow, {17 + 3, HOME_SPOT, HOME_SPOT, HOME_SPOT}},
            {PksColor::Red, {HOME_SPOT, HOME_SPOT, HOME_SPOT, 0}},
            {PksColor::Green, ALL_AT_HOME},
            {PksColor::Blue, ALL_AT_HOME},
        };

    // 2 possibilities as either order works
    REQUIRE(PksSnitchablePieces::getCapturingMoves(board, PksColor::Red, {1, 2}) == CapturingMoveSequences{
            {{3, 1, 0}, {3, 2, 1}},
            {{3, 2, 0}, {3, 1, 1}},
            });
}

TEST_CASE("Capture intermediate piece, needs both dice") {
    const auto board = PksPiecesByPlayer{
                {PksColor::Yellow, {17 + 3, 17 + 2, HOME_SPOT, HOME_SPOT}},
                {PksColor::Red, {HOME_SPOT, HOME_SPOT, HOME_SPOT, 0}},
                {PksColor::Green, ALL_AT_HOME},
                {PksColor::Blue, ALL_AT_HOME},
            };

    // 2 possibilities as either order works
    REQUIRE(PksSnitchablePieces::getCapturingMoves(board, PksColor::Red, {1, 2}) == CapturingMoveSequences{
            {{3, 1, 0}, {3, 2, 1}},
            {{3, 2, 1}, {3, 1, 1}},
            });
}

TEST_CASE("Two pieces can capture the same piece in alternative moves") {
    const auto board = PksPiecesByPlayer{
            {PksColor::Yellow, {17 + 1, 17 + 2, HOME_SPOT, HOME_SPOT}},
            {PksColor::Red, {HOME_SPOT, 0, HOME_SPOT, 0}},
            {PksColor::Green, ALL_AT_HOME},
            {PksColor::Blue, ALL_AT_START},
        };

    REQUIRE(PksSnitchablePieces::getCapturingMoves(board, PksColor::Red, {4, 2}) == CapturingMoveSequences{
            {{1, 2, 1}},
            {{3, 2, 1}},
            });
}

TEST_CASE("Captures 1 piece with double dice") {
    const auto board = PksPiecesByPlayer{
                    {PksColor::Yellow, {17 + 1, 17 + 2, 17 + 3, HOME_SPOT}},
                    {PksColor::Red, {HOME_SPOT, 0, HOME_SPOT, HOME_SPOT}},
                    {PksColor::Green, ALL_AT_HOME},
                    {PksColor::Blue, ALL_AT_START},
                };

    REQUIRE(PksSnitchablePieces::getCapturingMoves(board, PksColor::Red, {2, 2}) == CapturingMoveSequences{
            {{1, 2, 1}},
            });
}

TEST_CASE("Captures with 2 pieces with double dices (one on the way to the second)") {
    const auto board = PksPiecesByPlayer{
                {PksColor::Yellow, {17 + 1, 17 + 2, HOME_SPOT, HOME_SPOT}},
                {PksColor::Red, {HOME_SPOT, 0, HOME_SPOT, HOME_SPOT}},
                {PksColor::Green, ALL_AT_HOME},
                {PksColor::Blue, ALL_AT_START},
            };

    REQUIRE(PksSnitchablePieces::getCapturingMoves(board, PksColor::Red, {1, 1}) == CapturingMoveSequences{
            {{1, 1, 1}, {1, 1, 1}},
            });
}
