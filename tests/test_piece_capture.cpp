#include <catch2/catch_test_macros.hpp>

#include "MockDiceRoller.h"
#include "PksGame.h"
#include "PksUtils.h"
#include "src/TestBoards.h"

TEST_CASE("A piece captures another one") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    PksBoardState initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {17, 18, 19, 20}},
            {PksColor::Red, {0, 0, 0, 0}},
            {PksColor::Green, {0, 0, 0, 0}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Red,
    };
    REQUIRE(game.start(initialBoard) == PksColor::Red);

    // Roll a dice and capture a piece
    mockDiceRoller.setNextRandomValues(1, 2);
    game.rollDice();
    game.useDice(1, 0);

    auto currentState = game.getCurrentBoardState();
    REQUIRE(currentState.piecesByPlayer[PksColor::Red] == std::vector{1, 0, 0, 0});

    // And the other player should have a piece at home now
    REQUIRE(currentState.piecesByPlayer[PksColor::Yellow] == std::vector{17, -1, 19, 20});
    REQUIRE(currentState.piecesByPlayer[PksColor::Green] == std::vector{0, 0, 0, 0});
    REQUIRE(currentState.piecesByPlayer[PksColor::Blue] == std::vector{0, 0, 0, 0});

    // Still the same player's turn
    REQUIRE(currentState.currentPlayer == PksColor::Red);

    // Use the other dice and capture yet more pieces
    game.useDice(2, 0);

    currentState = game.getCurrentBoardState();
    REQUIRE(currentState.piecesByPlayer[PksColor::Red] == std::vector{3, 0, 0, 0});

    REQUIRE(currentState.piecesByPlayer[PksColor::Yellow] == std::vector{17, -1, 19, -1});
    REQUIRE(currentState.piecesByPlayer[PksColor::Green] == std::vector{0, 0, 0, 0});
    REQUIRE(currentState.piecesByPlayer[PksColor::Blue] == std::vector{0, 0, 0, 0});

    // Next player's turn
    REQUIRE(currentState.currentPlayer == PksColor::Green);
}

TEST_CASE("A piece captures multiple pieces of the same color") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    PksBoardState initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, 0, 0, 0}},
            {PksColor::Red, {0, 0, 0, 0}},
            {PksColor::Green, {0, 52, 52, 0}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Red,
    };
    REQUIRE(game.start(initialBoard) == PksColor::Red);

    // Roll a dice and capture a piece
    mockDiceRoller.setNextRandomValues(1, 2);
    game.rollDice();
    game.useDice(1, 0);

    auto currentState = game.getCurrentBoardState();
    REQUIRE(currentState.piecesByPlayer[PksColor::Red] == std::vector{1, 0, 0, 0});

    // And the other player should have 2 pieces at home now
    REQUIRE(currentState.piecesByPlayer[PksColor::Yellow] == std::vector{0, 0, 0, 0});
    REQUIRE(currentState.piecesByPlayer[PksColor::Green] == std::vector{0, -1, -1, 0});
    REQUIRE(currentState.piecesByPlayer[PksColor::Blue] == std::vector{0, 0, 0, 0});
}

TEST_CASE("A piece captures multiple pieces of different colors") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    PksBoardState initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {17, 18, 19, 0}},
            {PksColor::Red, {0, 0, 0, 0}},
            {PksColor::Green, {51, 52, 53, 0}},
            {PksColor::Blue, {34, 35, 36, 0}},
        },
        .currentPlayer = PksColor::Red,
    };
    REQUIRE(game.start(initialBoard) == PksColor::Red);

    // Roll a dice and capture a piece
    mockDiceRoller.setNextRandomValues(1, 2);
    game.rollDice();
    game.useDice(1, 0);

    auto currentState = game.getCurrentBoardState();
    REQUIRE(currentState.piecesByPlayer[PksColor::Red] == std::vector{1, 0, 0, 0});

    // And the other players should have some pieces at home now
    REQUIRE(currentState.piecesByPlayer[PksColor::Yellow] == std::vector{17, -1, 19, 0});
    REQUIRE(currentState.piecesByPlayer[PksColor::Green] == std::vector{51, -1, 53, 0});
    REQUIRE(currentState.piecesByPlayer[PksColor::Blue] == std::vector{34, -1, 36, 0});
}

TEST_CASE("Cannot capture pieces in safe spots") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    PksBoardState initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {23, 0, 0, 0}},
            {PksColor::Red, {7, 0, 0, 0}},
            {PksColor::Green, {0, 0, 0, 0}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Yellow,
    };
    REQUIRE(game.start(initialBoard) == PksColor::Yellow);

    // Roll a dice and reach the same spot where another player has a piece
    mockDiceRoller.setNextRandomValues(1, 2);
    game.rollDice();
    game.useDice(1, 0);

    auto currentState = game.getCurrentBoardState();
    REQUIRE(currentState.piecesByPlayer[PksColor::Yellow] == std::vector{24, 0, 0, 0}); // the piece moved normally
    REQUIRE(currentState.piecesByPlayer[PksColor::Red] == std::vector{7, 0, 0, 0}); // the other piece was not affected

    // Ensure the Yellow piece is at the same spot where the Red piece is
    const int yellowSpot = currentState.piecesByPlayer[PksColor::Yellow][0];
    const int redSpot = currentState.piecesByPlayer[PksColor::Red][0];
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Yellow, PksColor::Red, yellowSpot) == redSpot);
}
