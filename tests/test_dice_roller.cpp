#include <catch2/catch_test_macros.hpp>

#include "MockDiceRoller.h"
#include "PksGame.h"
#include "src/TestBoards.h"


TEST_CASE("Can roll and use dice (when not at home)") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    // The current player is not at Home
    PksBoardState initialBoard = {
        .piecesByPlayer = TestBoards::allPiecesAtStart(),
        .currentPlayer = PksColor::Yellow,
    };
    auto gameState = game.start(initialBoard);
    REQUIRE(gameState.currentPlayer == PksColor::Yellow);

    // Roll a "random" dice
    mockDiceRoller.setNextRandomValues(2, 3);
    auto diceRoll = game.rollDice();
    REQUIRE(!diceRoll.allDiceUsed());
    REQUIRE(diceRoll.getDice() == std::pair{2, 3});

    // Can't roll dice again until we use them
    CHECK_THROWS(game.rollDice());

    // Attempt to use an invalid dice
    CHECK_THROWS(game.useDice(1, 0));

    // Use a good dice
    gameState = game.useDice(2, 0);
    REQUIRE(gameState.currentPlayer == PksColor::Yellow);

    // And then the other
    gameState = game.useDice(3, 0);
    REQUIRE(gameState.currentPlayer == PksColor::Red); // the player changes
}

TEST_CASE("Can't get out of home without doubles") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    // The current player is not at Home
    PksBoardState initialBoard = {
        .piecesByPlayer = TestBoards::allPiecesAtHome(),
        .currentPlayer = PksColor::Yellow,
    };
    auto gameState = game.start(initialBoard);
    REQUIRE(gameState.currentPlayer == PksColor::Yellow);

    // Roll a "random" dice
    mockDiceRoller.setNextRandomValues(2, 3);
    auto diceRoll = game.rollDice();
    REQUIRE(diceRoll.allDiceUsed()); // can't use this roll as it's not doubles
    REQUIRE(diceRoll.getDice() == std::pair{2, 3});

    // Second chance
    mockDiceRoller.setNextRandomValues(2, 1);
    diceRoll = game.rollDice();
    REQUIRE(diceRoll.allDiceUsed()); // can't use this roll as it's not doubles
    REQUIRE(diceRoll.getDice() == std::pair{2, 1});

    // Last chance
    mockDiceRoller.setNextRandomValues(6, 5);
    diceRoll = game.rollDice();
    REQUIRE(diceRoll.allDiceUsed()); // can't use this roll as it's not doubles
    REQUIRE(diceRoll.getDice() == std::pair{6, 5});

    // The player failed to get out of Home, next player's turn
    REQUIRE(game.getCurrentBoardState().currentPlayer == PksColor::Red);
}

TEST_CASE("Can get out of home with doubles (all pieces at home)") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    // The current player is not at Home
    PksBoardState initialBoard = {
        .piecesByPlayer = TestBoards::allPiecesAtHome(),
        .currentPlayer = PksColor::Yellow,
    };
    auto gameState = game.start(initialBoard);
    REQUIRE(gameState.currentPlayer == PksColor::Yellow);

    // Roll a "random" dice
    mockDiceRoller.setNextRandomValues(2, 2);
    auto diceRoll = game.rollDice();
    REQUIRE(diceRoll.allDiceUsed()); // used to get out of home
    REQUIRE(diceRoll.getDice() == std::pair{2, 2});

    // Now is next player's turn
    REQUIRE(game.getCurrentBoardState().currentPlayer == PksColor::Red);
}

TEST_CASE("Can get out of home with doubles (only some pieces at home)") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    PksBoardState initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, 0, 0, 0}},
            {PksColor::Red, {0, 0, 0, 0}},
            {PksColor::Green, {1, -1, 1, 1}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Green,
    };
    auto gameState = game.start(initialBoard);
    REQUIRE(gameState.currentPlayer == PksColor::Green);

    // Roll a "random" dice
    mockDiceRoller.setNextRandomValues(2, 2);
    auto diceRoll = game.rollDice();
    REQUIRE(diceRoll.allDiceUsed()); // used to get out of home
    REQUIRE(diceRoll.getDice() == std::pair{2, 2});

    gameState = game.getCurrentBoardState();
    REQUIRE(gameState.piecesByPlayer[PksColor::Green] == std::vector{1, 0, 1, 1}); // got out of home

    // Next player's turn
    REQUIRE(gameState.currentPlayer == PksColor::Blue);
}

TEST_CASE("Can roll doubles (when not at home)") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    PksBoardState initialBoard = {
        .piecesByPlayer = TestBoards::allPiecesAtStart(),
        .currentPlayer = PksColor::Yellow,
    };
    auto gameState = game.start(initialBoard);
    REQUIRE(gameState.currentPlayer == PksColor::Yellow);

    // Roll a "random" dice
    mockDiceRoller.setNextRandomValues(4, 4);
    auto diceRoll = game.rollDice();
    REQUIRE(!diceRoll.allDiceUsed());
    REQUIRE(diceRoll.getDice() == std::pair{4, 4});

    // Can't roll dice again until we use them
    CHECK_THROWS(game.rollDice());

    // Attempt to use an invalid dice
    CHECK_THROWS(game.useDice(1, 0));

    // Use a good dice
    gameState = game.useDice(4, 0);
    REQUIRE(gameState.currentPlayer == PksColor::Yellow);

    // And then the other
    gameState = game.useDice(4, 0);
    // the player doesn't change, they need to roll again
    REQUIRE(gameState.currentPlayer == PksColor::Yellow);
}

TEST_CASE("Can roll doubles many times") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    PksBoardState initialBoard = {
        .piecesByPlayer = TestBoards::allPiecesAtStart(),
        .currentPlayer = PksColor::Yellow,
    };
    auto gameState = game.start(initialBoard);
    REQUIRE(gameState.currentPlayer == PksColor::Yellow);

    // Roll a "random" dice
    mockDiceRoller.setNextRandomValues(1, 1);
    auto diceRoll = game.rollDice();
    REQUIRE(!diceRoll.allDiceUsed());
    REQUIRE(diceRoll.getDice() == std::pair{1, 1});

    // Use them both
    gameState = game.useDice(1, 0);
    REQUIRE(gameState.currentPlayer == PksColor::Yellow); // still need to use the second dice
    gameState = game.useDice(1, 0);
    REQUIRE(gameState.currentPlayer == PksColor::Yellow); // can roll again

    // Roll more doubles and use them
    mockDiceRoller.setNextRandomValues(2, 2);
    diceRoll = game.rollDice();
    REQUIRE(!diceRoll.allDiceUsed());
    REQUIRE(diceRoll.getDice() == std::pair{2, 2});

    gameState = game.useDice(2, 0);
    REQUIRE(gameState.currentPlayer == PksColor::Yellow);
    gameState = game.useDice(2, 0);
    REQUIRE(gameState.currentPlayer == PksColor::Yellow); // can roll again

    // Roll doubles one last time, the player loses their turn after this
    mockDiceRoller.setNextRandomValues(3, 3);
    diceRoll = game.rollDice();
    REQUIRE(!diceRoll.allDiceUsed());
    REQUIRE(diceRoll.getDice() == std::pair{3, 3});
    REQUIRE(game.getCurrentBoardState().currentPlayer == PksColor::Yellow); // still the player's turn

    gameState = game.useDice(3, 0);
    REQUIRE(gameState.currentPlayer == PksColor::Yellow);
    gameState = game.useDice(3, 0);
    REQUIRE(gameState.currentPlayer == PksColor::Red); // it's another player's turn
}
