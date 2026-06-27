#include <catch2/catch_test_macros.hpp>

#include "MockDiceRoller.h"
#include "PksConstants.h"
#include "PksGame.h"
#include "src/TestBoards.h"

TEST_CASE("Can snitch if first dice wasted") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, 0, 17 + 19, 0}},
            {PksColor::Red, {10, 15, 1, 30}}, // piece 1, 15, can capture
            {PksColor::Green, {0, 0, 0, 0}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Red,
    };
    auto gameSnapshot = game.start(initialBoard);

    mockDiceRoller->setNextRandomValues(2, 4);
    gameSnapshot = game.rollDice();

    // Ensure the optimal moves are as we expect
    REQUIRE(gameSnapshot.optimalMoves == PksDMoveSet{
            { {1, 4, 1} },
            });

    REQUIRE(gameSnapshot.snitchablePieces.empty()); // not snitchable yet, Red can still attack
    REQUIRE(!game.snitchOnPlayer(PksColor::Red, {1})); // can't snitch yet

    gameSnapshot = game.useDice(4, 0);
    REQUIRE(gameSnapshot.snitchablePieces == std::set{1}); // Red lost the chance to attack, now can be snitched

    REQUIRE(!game.snitchOnPlayer(PksColor::Red, {2})); // can't snitch on wrong piece
    REQUIRE(!game.snitchOnPlayer(PksColor::Blue, {1})); // can't snitch on wrong player
    REQUIRE(game.snitchOnPlayer(PksColor::Red, {1})); // snitch successful!

    // Red piece 1 should be back at home, dice wasted on 0 still has effect
    gameSnapshot = game.getGameSnapshot();
    REQUIRE(gameSnapshot.piecesByPlayer.at(PksColor::Red) == std::vector<int>{14, HOME_SPOT, 1, 30});
}

TEST_CASE("Can snitch if second dice wasted") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, 0, 17 + 19, 0}},
            {PksColor::Red, {10, 15, 0, 30}}, // piece 1, 15, can capture
            {PksColor::Green, {0, 0, 0, 0}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Red,
    };
    auto gameSnapshot = game.start(initialBoard);

    mockDiceRoller->setNextRandomValues(2, 4);
    gameSnapshot = game.rollDice();

    // Ensure the optimal moves are as we expect
    REQUIRE(gameSnapshot.optimalMoves == PksDMoveSet{
            { {1, 4, 1} },
            });

    // Waste a dice, but it wasn't necessary for the optimal move
    gameSnapshot = game.useDice(2, 0);
    REQUIRE(gameSnapshot.snitchablePieces.empty()); // not snitchable yet, Red can still attack
    REQUIRE(!game.snitchOnPlayer(PksColor::Red, {1})); // can't snitch yet

    // Waste the important dice now, Red lost the chance to capture the Yellow piece
    gameSnapshot = game.useDice(4, 0);
    REQUIRE(gameSnapshot.snitchablePieces == std::set{1});
    REQUIRE(game.snitchOnPlayer(PksColor::Red, {1})); // snitch successful!

    // Red piece 1 should be back at home, 2 dice used on 0 still have effect
    gameSnapshot = game.getGameSnapshot();
    REQUIRE(gameSnapshot.piecesByPlayer.at(PksColor::Red) == std::vector<int>{16, HOME_SPOT, 0, 30});
}

TEST_CASE("Can snitch if first dice wasted and both dice were required (2 pieces snitched)") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, 17 + 1, 17 + 2, 0}},
            {PksColor::Red, {0, 1, 2, 0}},
            {PksColor::Green, {0, 0, 0, 0}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Red,
    };
    auto gameSnapshot = game.start(initialBoard);

    mockDiceRoller->setNextRandomValues(1, 2);
    gameSnapshot = game.rollDice();

    // Ensure the optimal moves are as we expect
    REQUIRE(gameSnapshot.optimalMoves == PksDMoveSet{
            { {0, 1, 1}, {3, 2, 1}},
            { {0, 2, 1}, {3, 1, 1}},
            // Same but inverted:
            { {3, 2, 1}, {0, 1, 1}},
            { {3, 1, 1}, {0, 2, 1}},
            });

    // Waste a dice, which was necessary for the optimal move
    gameSnapshot = game.useDice(2, 1);
    REQUIRE(gameSnapshot.snitchablePieces == std::set{0, 3});
    REQUIRE(game.snitchOnPlayer(PksColor::Red, {0, 3})); // snitch successful!

    // Red pieces should be back at home
    gameSnapshot = game.getGameSnapshot();
    REQUIRE(gameSnapshot.piecesByPlayer.at(PksColor::Red) == std::vector<int>{HOME_SPOT, 3, 2, HOME_SPOT});
}

TEST_CASE("Can snitch if first dice used ok but second dice wasted") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
                {PksColor::Yellow, {0, 17 + 1, 17 + 2, 0}},
                {PksColor::Red, {0, 1, 2, 0}},
                {PksColor::Green, {0, 0, 0, 0}},
                {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Red,
    };
    auto gameSnapshot = game.start(initialBoard);

    // Optimal play is: [{P0, D1}, {P0, D2}]
    mockDiceRoller->setNextRandomValues(1, 2);
    gameSnapshot = game.rollDice();

    // Ensure the optimal moves are as we expect
    REQUIRE(gameSnapshot.optimalMoves == PksDMoveSet{
            { {0, 1, 1}, {3, 2, 1}},
            { {0, 2, 1}, {3, 1, 1}},
            // Same but inverted:
            { {3, 2, 1}, {0, 1, 1}},
            { {3, 1, 1}, {0, 2, 1}},
            });

    // Use a dice wisely
    gameSnapshot = game.useDice(1, 0);
    REQUIRE(gameSnapshot.snitchablePieces.empty());
    REQUIRE(!game.snitchOnPlayer(PksColor::Red, {0, 3}));

    // Use the second dice unwisely
    gameSnapshot = game.useDice(2, 1);
    REQUIRE(gameSnapshot.snitchablePieces == std::set{0, 3});
    REQUIRE(game.snitchOnPlayer(PksColor::Red, {0, 3})); // snitch successful!

    // Red pieces should be back at home
    gameSnapshot = game.getGameSnapshot();
    REQUIRE(gameSnapshot.piecesByPlayer.at(PksColor::Red) == std::vector<int>{HOME_SPOT, 3, 2, HOME_SPOT});
}


TEST_CASE("Can only snitch before next dice roll") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
                {PksColor::Yellow, {0, 0, 17 + 19, 0}},
                {PksColor::Red, {10, 15, 1, 30}}, // piece 1, 15, can capture
                {PksColor::Green, {0, 0, 0, 0}},
                {PksColor::Blue, {0, 0, 0, 0}},
            },
            .currentPlayer = PksColor::Red,
        };
    auto gameSnapshot = game.start(initialBoard);

    mockDiceRoller->setNextRandomValues(2, 4);
    gameSnapshot = game.rollDice();

    // Ensure the optimal moves are as we expect
    REQUIRE(gameSnapshot.optimalMoves == PksDMoveSet{
            { {1, 4, 1} },
            });

    gameSnapshot = game.useDice(4, 0);
    REQUIRE(gameSnapshot.snitchablePieces == std::set{1});
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Red);

    // No other player has snitched, and red uses the second dice
    gameSnapshot = game.useDice(2, 0);
    REQUIRE(gameSnapshot.snitchablePieces == std::set{1});
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Green); // next player can roll

    // Nobody noticed, and the next player rolls
    mockDiceRoller->setNextRandomValues(1, 1);
    game.rollDice();

    gameSnapshot = game.getGameSnapshot();
    REQUIRE(gameSnapshot.snitchablePieces.empty()); // Red can't be snitched anymore
}
