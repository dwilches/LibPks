#include <catch2/catch_test_macros.hpp>

#include "MockDiceRoller.h"
#include "PksGame.h"
#include "src/TestBoards.h"


TEST_CASE("Can roll and use dice (when not at home)") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    // The current player is not at Home
    PksBoardState initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, 0, 0, 0}},
            {PksColor::Red, {-1, -1, -1, -1}},
            {PksColor::Green, {-1, -1, -1, -1}},
            {PksColor::Blue, {-1, -1, -1, -1}},
        },
        .currentPlayer = PksColor::Yellow,
    };
    REQUIRE(game.start(initialBoard) == PksColor::Yellow);

    // Roll a "random" dice
    mockDiceRoller.setNextRandomValues(2, 3);
    auto diceRoll = game.rollDice();
    REQUIRE(diceRoll.first == RolledDice{.value=2, .alreadyUsed=false});
    REQUIRE(diceRoll.second == RolledDice{.value=3, .alreadyUsed=false});

    // Can't roll dice again until we use them
    CHECK_THROWS(game.rollDice());

    // Attempt to use an invalid dice
    CHECK_THROWS(game.useDice(1, 0));

    // Use a good dice
    REQUIRE(game.useDice(2, 0) == PksColor::Yellow);
    REQUIRE(game.getCurrentBoardState().currentPlayer == PksColor::Yellow);

    // And then the other
    REQUIRE(game.useDice(3, 0) == PksColor::Red); // the player changes
    REQUIRE(game.getCurrentBoardState().currentPlayer == PksColor::Red);
}

TEST_CASE("Can't get out of home without doubles") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    // The current player is not at Home
    PksBoardState initialBoard = {
        .piecesByPlayer = TestBoards::allPiecesAtHome(),
        .currentPlayer = PksColor::Yellow,
    };
    REQUIRE(game.start(initialBoard) == PksColor::Yellow);

    // Roll a "random" dice
    mockDiceRoller.setNextRandomValues(2, 3);
    auto diceRoll = game.rollDice();
    REQUIRE(diceRoll.first == RolledDice{.value=2, .alreadyUsed=true}); // can't use this roll as it's not doubles
    REQUIRE(diceRoll.second == RolledDice{.value=3, .alreadyUsed=true}); // can't use this roll as it's not doubles

    // Second chance
    mockDiceRoller.setNextRandomValues(2, 1);
    diceRoll = game.rollDice();
    REQUIRE(diceRoll.first == RolledDice{.value=2, .alreadyUsed=true}); // can't use this roll as it's not doubles
    REQUIRE(diceRoll.second == RolledDice{.value=1, .alreadyUsed=true}); // can't use this roll as it's not doubles

    // Last chance
    mockDiceRoller.setNextRandomValues(6, 5);
    diceRoll = game.rollDice();
    REQUIRE(diceRoll.first == RolledDice{.value=6, .alreadyUsed=true}); // can't use this roll as it's not doubles
    REQUIRE(diceRoll.second == RolledDice{.value=5, .alreadyUsed=true}); // can't use this roll as it's not doubles

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
    REQUIRE(game.start(initialBoard) == PksColor::Yellow);

    // Roll a "random" dice
    mockDiceRoller.setNextRandomValues(2, 2);
    auto diceRoll = game.rollDice();
    REQUIRE(diceRoll.first == RolledDice{.value=2, .alreadyUsed=true}); // used to get out of home
    REQUIRE(diceRoll.second == RolledDice{.value=2, .alreadyUsed=true}); // used to get out of home

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
    REQUIRE(game.start(initialBoard) == PksColor::Green);

    // Roll a "random" dice
    mockDiceRoller.setNextRandomValues(2, 2);
    auto diceRoll = game.rollDice();
    REQUIRE(diceRoll.first == RolledDice{.value=2, .alreadyUsed=true}); // used to get out of home
    REQUIRE(diceRoll.second == RolledDice{.value=2, .alreadyUsed=true}); // used to get out of home

    auto currentState = game.getCurrentBoardState();
    REQUIRE(currentState.piecesByPlayer[PksColor::Green] == std::vector{1, 0, 1, 1}); // got out of home

    // Next player's turn
    REQUIRE(currentState.currentPlayer == PksColor::Blue);
}

TEST_CASE("Can roll doubles (when not at home)") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    PksBoardState initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, 0, 0, 0}},
            {PksColor::Red, {-1, -1, -1, -1}},
            {PksColor::Green, {-1, -1, -1, -1}},
            {PksColor::Blue, {-1, -1, -1, -1}},
        },
        .currentPlayer = PksColor::Yellow,
    };
    REQUIRE(game.start(initialBoard) == PksColor::Yellow);

    // Roll a "random" dice
    mockDiceRoller.setNextRandomValues(4, 4);
    auto diceRoll = game.rollDice();
    REQUIRE(diceRoll.first == RolledDice{.value=4, .alreadyUsed=false});
    REQUIRE(diceRoll.second == RolledDice{.value=4, .alreadyUsed=false});

    // Can't roll dice again until we use them
    CHECK_THROWS(game.rollDice());

    // Attempt to use an invalid dice
    CHECK_THROWS(game.useDice(1, 0));

    // Use a good dice
    REQUIRE(game.useDice(4, 0) == PksColor::Yellow);
    REQUIRE(game.getCurrentBoardState().currentPlayer == PksColor::Yellow);

    // And then the other
    REQUIRE(game.useDice(4, 0) == PksColor::Yellow); // the player doesn't change, they need to roll again
    REQUIRE(game.getCurrentBoardState().currentPlayer == PksColor::Yellow); // can roll again
}

TEST_CASE("Can roll doubles many times") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    PksBoardState initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, 0, 0, 0}},
            {PksColor::Red, {-1, -1, -1, -1}},
            {PksColor::Green, {-1, -1, -1, -1}},
            {PksColor::Blue, {-1, -1, -1, -1}},
        },
        .currentPlayer = PksColor::Yellow,
    };
    REQUIRE(game.start(initialBoard) == PksColor::Yellow);

    // Roll a "random" dice
    mockDiceRoller.setNextRandomValues(1, 1);
    auto diceRoll = game.rollDice();
    REQUIRE(diceRoll.first == RolledDice{.value=1, .alreadyUsed=false});
    REQUIRE(diceRoll.second == RolledDice{.value=1, .alreadyUsed=false});

    // Use them both
    REQUIRE(game.useDice(1, 0) == PksColor::Yellow);
    REQUIRE(game.useDice(1, 0) == PksColor::Yellow);
    REQUIRE(game.getCurrentBoardState().currentPlayer == PksColor::Yellow); // can roll again

    // Roll more doubles and use them
    mockDiceRoller.setNextRandomValues(2, 2);
    diceRoll = game.rollDice();
    REQUIRE(diceRoll.first == RolledDice{.value=2, .alreadyUsed=false});
    REQUIRE(diceRoll.second == RolledDice{.value=2, .alreadyUsed=false});

    REQUIRE(game.useDice(2, 0) == PksColor::Yellow);
    REQUIRE(game.useDice(2, 0) == PksColor::Yellow);
    REQUIRE(game.getCurrentBoardState().currentPlayer == PksColor::Yellow); // can roll again

    // Roll doubles one last time, the player loses their turn after this
    mockDiceRoller.setNextRandomValues(3, 3);
    diceRoll = game.rollDice();
    REQUIRE(diceRoll.first == RolledDice{.value=3, .alreadyUsed=false});
    REQUIRE(diceRoll.second == RolledDice{.value=3, .alreadyUsed=false});
    REQUIRE(game.getCurrentBoardState().currentPlayer == PksColor::Yellow); // still the player's turn

    REQUIRE(game.useDice(3, 0) == PksColor::Yellow);
    REQUIRE(game.useDice(3, 0) == PksColor::Red); // not the original player anymore
    REQUIRE(game.getCurrentBoardState().currentPlayer == PksColor::Red); // it's another player's turn
}
