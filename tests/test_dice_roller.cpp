#include <catch2/catch_test_macros.hpp>

#include "MockDiceRoller.h"
#include "PksConstants.h"
#include "PksGame.h"
#include "src/TestBoards.h"

bool allDiceUsed(const PksGameSnapshot& snapshot) {
    return !snapshot.isDiceUsable.first && !snapshot.isDiceUsable.second;
}

TEST_CASE("Can roll and use dice (when not at home)") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    // The current player is not at Home
    PksGameSnapshot initialBoard = {
        .piecesByPlayer = TestBoards::allPiecesAtStart(),
        .currentPlayer = PksColor::Yellow,
    };
    auto gameSnapshot = game.start(initialBoard);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);

    // Roll a "random" dice
    mockDiceRoller->setNextRandomValues(2, 3);
    gameSnapshot = game.rollDice();
    REQUIRE(!allDiceUsed(gameSnapshot));
    REQUIRE(gameSnapshot.diceValues == std::pair{2, 3});

    // Can't roll dice again until we use them
    CHECK_THROWS(game.rollDice());

    // Attempt to use an invalid dice
    CHECK_THROWS(game.useDice(1, 0));

    // Use a good dice
    gameSnapshot = game.useDice(2, 0);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);

    // And then the other
    gameSnapshot = game.useDice(3, 0);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Red); // the player changes
}

TEST_CASE("Can't get out of home without doubles") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    // The current player is not at Home
    PksGameSnapshot initialBoard = {
        .piecesByPlayer = TestBoards::allPiecesAtHome(),
        .currentPlayer = PksColor::Yellow,
    };
    auto gameSnapshot = game.start(initialBoard);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);

    // Roll a "random" dice
    mockDiceRoller->setNextRandomValues(2, 3);
    gameSnapshot = game.rollDice();
    REQUIRE(allDiceUsed(gameSnapshot)); // can't use this roll as it's not doubles
    REQUIRE(gameSnapshot.diceValues == std::pair{2, 3});

    // Second chance
    mockDiceRoller->setNextRandomValues(2, 1);
    gameSnapshot = game.rollDice();
    REQUIRE(allDiceUsed(gameSnapshot)); // can't use this roll as it's not doubles
    REQUIRE(gameSnapshot.diceValues == std::pair{2, 1});

    // Last chance
    mockDiceRoller->setNextRandomValues(6, 5);
    gameSnapshot = game.rollDice();
    REQUIRE(allDiceUsed(gameSnapshot)); // can't use this roll as it's not doubles
    REQUIRE(gameSnapshot.diceValues == std::pair{6, 5});

    // The player failed to get out of Home, next player's turn
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Red);
}

TEST_CASE("Can get out of home with doubles (all pieces at home)") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    // The current player is not at Home
    PksGameSnapshot initialBoard = {
        .piecesByPlayer = TestBoards::allPiecesAtHome(),
        .currentPlayer = PksColor::Yellow,
    };
    auto gameSnapshot = game.start(initialBoard);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);

    // Roll a "random" dice
    mockDiceRoller->setNextRandomValues(2, 2);
    gameSnapshot = game.rollDice();
    REQUIRE(allDiceUsed(gameSnapshot)); // used to get out of home
    REQUIRE(gameSnapshot.diceValues == std::pair{2, 2});

    // Now is next player's turn
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Red);
}

TEST_CASE("Can get out of home with doubles (only some pieces at home)") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, 0, 0, 0}},
            {PksColor::Red, {0, 0, 0, 0}},
            {PksColor::Green, {1, -1, 1, 1}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Green,
    };
    auto gameSnapshot = game.start(initialBoard);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Green);

    // Roll a "random" dice
    mockDiceRoller->setNextRandomValues(2, 2);
    gameSnapshot = game.rollDice();
    REQUIRE(allDiceUsed(gameSnapshot)); // used to get out of home
    REQUIRE(gameSnapshot.diceValues == std::pair{2, 2});

    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Green] == std::vector{1, 0, 1, 1}); // got out of home

    // Next player's turn
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Blue);
}

TEST_CASE("Can roll doubles (when not at home)") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = TestBoards::allPiecesAtStart(),
        .currentPlayer = PksColor::Yellow,
    };
    auto gameSnapshot = game.start(initialBoard);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);

    // Roll a "random" dice
    mockDiceRoller->setNextRandomValues(4, 4);
    gameSnapshot = game.rollDice();
    REQUIRE(!allDiceUsed(gameSnapshot));
    REQUIRE(gameSnapshot.diceValues == std::pair{4, 4});

    // Can't roll dice again until we use them
    CHECK_THROWS(game.rollDice());

    // Attempt to use an invalid dice
    CHECK_THROWS(game.useDice(1, 0));

    // Use a good dice
    gameSnapshot = game.useDice(4, 0);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);

    // And then the other
    gameSnapshot = game.useDice(4, 0);
    // the player doesn't change, they need to roll again
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);
}

TEST_CASE("Can roll doubles twice in a row") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = TestBoards::allPiecesAtStart(),
        .currentPlayer = PksColor::Yellow,
    };
    auto gameSnapshot = game.start(initialBoard);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);

    // Roll a "random" dice
    mockDiceRoller->setNextRandomValues(1, 1);
    gameSnapshot = game.rollDice();
    REQUIRE(!allDiceUsed(gameSnapshot));
    REQUIRE(gameSnapshot.diceValues == std::pair{1, 1});

    // Use them both
    gameSnapshot = game.useDice(1, 0);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow); // still need to use the second dice
    gameSnapshot = game.useDice(1, 0);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow); // can roll again

    // Roll more doubles and use them
    mockDiceRoller->setNextRandomValues(2, 2);
    gameSnapshot = game.rollDice();
    REQUIRE(!allDiceUsed(gameSnapshot));
    REQUIRE(gameSnapshot.diceValues == std::pair{2, 2});

    gameSnapshot = game.useDice(2, 0);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);
    gameSnapshot = game.useDice(2, 0);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow); // can roll again

    // Roll dice one last time, it's not doubles this time
    mockDiceRoller->setNextRandomValues(3, 1);
    gameSnapshot = game.rollDice();
    REQUIRE(!allDiceUsed(gameSnapshot));
    REQUIRE(gameSnapshot.diceValues == std::pair{3, 1});

    gameSnapshot = game.useDice(3, 0);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);
    gameSnapshot = game.useDice(1, 0);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Red); // it's another player's turn
}

TEST_CASE("Three consecutive doubles send all playing pieces homes") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, START_SPOT, FINAL_TARGET_SPOT, 10}},
            {PksColor::Red, {0, 0, 0, 0}},
            {PksColor::Green, {0, 0, 0, 0}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Yellow,
    };
    auto gameSnapshot = game.start(initialBoard);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);

    // Roll doubles and use them
    mockDiceRoller->setNextRandomValues(1, 1);
    game.rollDice();

    game.useDice(1, 0);
    game.useDice(1, 0);

    // Roll more doubles and use them again
    mockDiceRoller->setNextRandomValues(2, 2);
    game.rollDice();
    game.useDice(2, 0);
    game.useDice(2, 0);

    // Roll doubles one last time, this causes all the player's pieces to be sent home, except those that are already
    // out of play
    mockDiceRoller->setNextRandomValues(3, 3);
    gameSnapshot = game.rollDice();
    REQUIRE(allDiceUsed(gameSnapshot)); // can't use this dice roll

    // Only the pieces that were in play are sent home
    // The piece that was out of play  is untouched
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] ==
            std::vector{HOME_SPOT, HOME_SPOT, FINAL_TARGET_SPOT, HOME_SPOT});
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Red); // it's another player's turn
}

TEST_CASE("Can't use a valid dice on a piece that is not in play") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    // The current player is not at Home
    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, HOME_SPOT, 0, FINAL_TARGET_SPOT}},
            {PksColor::Red, {-1, -1, -1, -1}},
            {PksColor::Green, {-1, -1, -1, -1}},
            {PksColor::Blue, {-1, -1, -1, -1}},
        },
        .currentPlayer = PksColor::Yellow,
    };
    auto gameSnapshot = game.start(initialBoard);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);

    // Roll a "random" dice
    mockDiceRoller->setNextRandomValues(2, 3);
    gameSnapshot = game.rollDice();
    REQUIRE_THROWS(game.useDice(2, 1)); // Piece at home
    REQUIRE_THROWS(game.useDice(2, 3)); // Piece already out of play

    // Board should be identical
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{0, HOME_SPOT, 0, FINAL_TARGET_SPOT});
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);
}

TEST_CASE("Can't use a dice that was already used") {
    auto mockDiceRoller = std::make_shared<MockDiceRoller>();
    PksGame game{mockDiceRoller};

    // The current player is not at Home
    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, HOME_SPOT, 0, FINAL_TARGET_SPOT}},
            {PksColor::Red, {-1, -1, -1, -1}},
            {PksColor::Green, {-1, -1, -1, -1}},
            {PksColor::Blue, {-1, -1, -1, -1}},
        },
        .currentPlayer = PksColor::Yellow,
    };
    auto gameSnapshot = game.start(initialBoard);
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Yellow);

    // Roll a "random" dice
    mockDiceRoller->setNextRandomValues(2, 3);
    game.rollDice();
    gameSnapshot = game.useDice(2, 0);
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{2, HOME_SPOT, 0, FINAL_TARGET_SPOT});

    // Try to use the same dice again, it should fail
    REQUIRE_THROWS(game.useDice(2, 0));
    REQUIRE(gameSnapshot.piecesByPlayer[PksColor::Yellow] == std::vector{2, HOME_SPOT, 0, FINAL_TARGET_SPOT});
}
