#include <catch2/catch_test_macros.hpp>

#include "MockDiceRoller.h"
#include "PksConstants.h"
#include "PksGame.h"
#include "PksUtils.h"
#include "src/TestBoards.h"

TEST_CASE("A piece captures another one") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {17, 18, 19, 20}},
            {PksColor::Red, {0, 0, 0, 0}},
            {PksColor::Green, {0, 0, 0, 0}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Red,
    };
    auto gameSnapshot = game.start(initialBoard);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Red);

    // Roll a dice and capture a piece
    mockDiceRoller->setNextRandomValues(1, 2);
    game.rollDice();
    gameSnapshot = game.useDice(1, 0);

    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Red] == std::vector{1, 0, 0, 0});

    // And the other player should have a piece at home now
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{17, -1, 19, 20});
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Green] == std::vector{0, 0, 0, 0});
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Blue] == std::vector{0, 0, 0, 0});

    // Still the same player's turn
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Red);

    // Use the other dice and capture yet more pieces
    gameSnapshot = game.useDice(2, 0);

    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Red] == std::vector{3, 0, 0, 0});

    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{17, -1, 19, -1});
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Green] == std::vector{0, 0, 0, 0});
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Blue] == std::vector{0, 0, 0, 0});

    // Next player's turn
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Green);
}

TEST_CASE("A piece captures multiple pieces of the same color") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, 0, 0, 0}},
            {PksColor::Red, {0, 0, 0, 0}},
            {PksColor::Green, {0, 52, 52, 0}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Red,
    };
    auto gameSnapshot = game.start(initialBoard);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Red);

    // Roll a dice and capture a piece
    mockDiceRoller->setNextRandomValues(1, 2);
    game.rollDice();
    gameSnapshot = game.useDice(1, 0);

    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Red] == std::vector{1, 0, 0, 0});

    // And the other player should have 2 pieces at home now
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{0, 0, 0, 0});
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Green] == std::vector{0, -1, -1, 0});
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Blue] == std::vector{0, 0, 0, 0});
}

TEST_CASE("A piece captures multiple pieces of different colors") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {17, 18, 19, 0}},
            {PksColor::Red, {0, 0, 0, 0}},
            {PksColor::Green, {51, 52, 53, 0}},
            {PksColor::Blue, {34, 35, 36, 0}},
        },
        .currentPlayer = PksColor::Red,
    };
    auto gameSnapshot = game.start(initialBoard);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Red);

    // Roll a dice and capture a piece
    mockDiceRoller->setNextRandomValues(1, 2);
    game.rollDice();
    gameSnapshot = game.useDice(1, 0);

    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Red] == std::vector{1, 0, 0, 0});

    // And the other players should have some pieces at home now
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{17, -1, 19, 0});
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Green] == std::vector{51, -1, 53, 0});
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Blue] == std::vector{34, -1, 36, 0});
}

TEST_CASE("Cannot capture pieces in safe spots") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {23, 0, 0, 0}},
            {PksColor::Red, {7, 0, 0, 0}},
            {PksColor::Green, {0, 0, 0, 0}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Yellow,
    };
    auto gameSnapshot = game.start(initialBoard);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);

    // Roll a dice and reach the same spot where another player has a piece
    mockDiceRoller->setNextRandomValues(1, 2);
    game.rollDice();
    gameSnapshot = game.useDice(1, 0);

    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{24, 0, 0, 0}); // the piece moved normally
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Red] == std::vector{7, 0, 0, 0}); // the other piece was not affected

    // Ensure the Yellow piece is at the same spot where the Red piece is
    const int yellowSpot = gameSnapshot.piecesByPlayer[PksColor::Yellow][0];
    const int redSpot = gameSnapshot.piecesByPlayer[PksColor::Red][0];
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Yellow, PksColor::Red, yellowSpot) == redSpot);
}

TEST_CASE("When going out of Home, capture foreign pieces currently at our Home") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
                {PksColor::Yellow, {0, 0, 0, HOME_SPOT}},
                {PksColor::Red, {51, 0, 0, 0}},
                {PksColor::Green, {0, 0, 0, 0}},
                {PksColor::Blue, {0, 0, 0, 0}},
            },
            .currentPlayer = PksColor::Yellow,
    };
    auto gameSnapshot = game.start(initialBoard);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);

    // Roll doubles to get out of home
    mockDiceRoller->setNextRandomValues(2, 2);
    game.rollDice();

    // All pieces at our home should have been captured
    gameSnapshot = game.getGameSnapshot();
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{0, 0, 0, 0}); // piece got out of home
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Red] == std::vector{HOME_SPOT, 0, 0, 0}); // foreign piece sent home
}
