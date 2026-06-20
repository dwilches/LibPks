#include <catch2/catch_test_macros.hpp>

#include "MockDiceRoller.h"
#include "PksGame.h"
#include "src/TestBoards.h"

TEST_CASE("All pieces move out of home") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    // The current player is not at Home
    PksGameSnapshot initialBoard = {
        .piecesByPlayer = TestBoards::allPiecesAtHome(),
        .currentPlayer = PksColor::Yellow,
    };
    auto gameSnapshot = game.start(initialBoard);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{-1, -1, -1, -1}); // all at home

    // Roll doubles to get out of home
    mockDiceRoller->setNextRandomValues(6, 6);
    auto diceRoll = game.rollDice();
    REQUIRE(diceRoll.allDiceUsed()); // used to get out of home
    REQUIRE(diceRoll.getDice() == PksDicePair{6, 6});

    gameSnapshot = game.getGameSnapshot();
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{0, 0, 0, 0}); // all at start
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Red); // Next player's turn
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Red] == std::vector{-1, -1, -1, -1}); // Next player is still at home
}

TEST_CASE("Pieces move according to dice value (same piece moves both dice)") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    // The current player is not at Home
    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, 10, 20, 30}},
            {PksColor::Red, {-1, -1, -1, -1}},
            {PksColor::Green, {-1, -1, -1, -1}},
            {PksColor::Blue, {-1, -1, -1, -1}},
        },
        .currentPlayer = PksColor::Yellow,
    };
    auto gameSnapshot = game.start(initialBoard);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{0, 10, 20, 30});

    // Roll a dice and ensure the pieces move the desired amounts
    mockDiceRoller->setNextRandomValues(6, 3);
    game.rollDice();

    gameSnapshot = game.getGameSnapshot();
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{0, 10, 20, 30});

    gameSnapshot = game.useDice(6, 1); // Should move from 10 to 16
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{0, 16, 20, 30});

    gameSnapshot = game.useDice(3, 1); // Should move from 16 to 19
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{0, 19, 20, 30});

    // Next player's turn
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Red);
}

TEST_CASE("Pieces move according to dice value (different pieces move)") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    // The current player is not at Home
    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, 10, 20, 30}},
            {PksColor::Red, {-1, -1, -1, -1}},
            {PksColor::Green, {-1, -1, -1, -1}},
            {PksColor::Blue, {-1, -1, -1, -1}},
        },
        .currentPlayer = PksColor::Yellow,
    };
    auto gameSnapshot = game.start(initialBoard);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);
    REQUIRE(game.getGameSnapshot().piecesByPlayer[PksColor::Yellow] == std::vector{0, 10, 20, 30});

    // Roll a dice and ensure the pieces move the desired amounts
    mockDiceRoller->setNextRandomValues(6, 3);
    game.rollDice();

    gameSnapshot = game.getGameSnapshot();
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{0, 10, 20, 30});

    gameSnapshot = game.useDice(6, 2); // Should move from 20 to 26
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{0, 10, 26, 30});

    gameSnapshot = game.useDice(3, 1); // Should move from 10 to 13
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{0, 13, 26, 30});

    // Next player's turn
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Red);
}

TEST_CASE("Pieces move according to dice value (doubles test)") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    // The current player is not at Home
    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, 10, 20, 30}},
            {PksColor::Red, {-1, -1, -1, -1}},
            {PksColor::Green, {-1, -1, -1, -1}},
            {PksColor::Blue, {-1, -1, -1, -1}},
        },
        .currentPlayer = PksColor::Yellow,
    };
    auto gameSnapshot = game.start(initialBoard);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);
    REQUIRE(game.getGameSnapshot().piecesByPlayer[PksColor::Yellow] == std::vector{0, 10, 20, 30});

    // Roll a dice and ensure the pieces move the desired amounts
    mockDiceRoller->setNextRandomValues(6, 6);
    game.rollDice();
    game.useDice(6, 0);
    gameSnapshot = game.useDice(6, 1);
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{6, 16, 20, 30});

    // Same player's turn, as it was doubles
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);

    // Roll another dice
    mockDiceRoller->setNextRandomValues(5, 5);
    game.rollDice();
    game.useDice(5, 1);
    gameSnapshot = game.useDice(5, 2);
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{6, 21, 25, 30});

    // Still, same player's turn, as it was doubles
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);

    // Roll another dice
    mockDiceRoller->setNextRandomValues(2, 1);
    game.rollDice();
    game.useDice(2, 1);
    gameSnapshot = game.useDice(1, 0);
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{7, 23, 25, 30});

    // Too many doubles, next player's turn
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Red);
}

TEST_CASE("Pieces move according to dice value (all players move)") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    // All pieces are at the start
    PksGameSnapshot initialBoard = {
        .piecesByPlayer = TestBoards::allPiecesAtStart(),
        .currentPlayer = PksColor::Yellow,
    };
    auto gameSnapshot = game.start(initialBoard);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);

    // Roll a dice and ensure the pieces move the desired amounts
    mockDiceRoller->setNextRandomValues(1, 2);
    game.rollDice();
    game.useDice(1, 0);
    gameSnapshot = game.useDice(2, 1);
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{1, 2, 0, 0});

    // Next player rolls a dice
    mockDiceRoller->setNextRandomValues(5, 3);
    game.rollDice();
    game.useDice(5, 1);
    gameSnapshot = game.useDice(3, 2);
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Red] == std::vector{0, 5, 3, 0});

    // Next player rolls a dice (doubles)
    mockDiceRoller->setNextRandomValues(3, 3);
    game.rollDice();
    game.useDice(3, 0);
    gameSnapshot = game.useDice(3, 1);
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Green] == std::vector{3, 3, 0, 0});

    // Same player can roll again
    mockDiceRoller->setNextRandomValues(2, 3);
    game.rollDice();
    game.useDice(2, 0);
    gameSnapshot=game.useDice(3, 1);
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Green] == std::vector{5, 6, 0, 0});

    // Last player rolls a dice
    mockDiceRoller->setNextRandomValues(6, 3);
    game.rollDice();
    game.useDice(3, 3);
    gameSnapshot=game.useDice(6, 3);
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Blue] == std::vector{0, 0, 0, 9});

    // The next player would be the first one
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);

    // Ensure the whole board is as we expect
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{1, 2, 0, 0});
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Red] == std::vector{0, 5, 3, 0});
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Green] == std::vector{5, 6, 0, 0});
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Blue] == std::vector{0, 0, 0, 9});
}
