#include <catch2/catch_test_macros.hpp>

#include "MockDiceRoller.h"
#include "PksConstants.h"
#include "PksGame.h"
#include "src/TestBoards.h"

/*
TEST_CASE("Validate attackable spots") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {START_SPOT, 10, 20, 50}},
            {PksColor::Red, {0, 0, 0, 0}},
            {PksColor::Green, {0, 0, 0, 0}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Yellow,
    };
    auto gameSnapshot = game.start(initialBoard);

    mockDiceRoller.setNextRandomValues(1, 2);
    game.rollDice();

    auto attackSpots = game.getAttackableSpots();
    REQUIRE(attackSpots[0] == std::set<int>{1, 2, 3});
    REQUIRE(attackSpots[1] == std::set<int>{11, 13}); // can't attack spot 12 as it's a safe one
    REQUIRE(attackSpots[2] == std::set<int>{21, 22, 23});
    REQUIRE(attackSpots[3] == std::set<int>{52, 53}); // can't attack spot 51 as it's safe (another player's home)
}

TEST_CASE("Validate can't attack from home nor final stair") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {HOME_SPOT, START_SPOT, LAST_SHARED_SPOT, FINAL_TARGET_SPOT}},
            {PksColor::Red, {0, 0, 0, 0}},
            {PksColor::Green, {0, 0, 0, 0}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Yellow,
    };
    auto gameSnapshot = game.start(initialBoard);

    mockDiceRoller.setNextRandomValues(2, 4);
    game.rollDice();

    auto attackSpots = game.getAttackableSpots();
    REQUIRE(!attackSpots.contains(0)); // can't attack from home
    REQUIRE(attackSpots[1] == std::set<int>{2, 4, 6});
    REQUIRE(!attackSpots.contains(2)); // can't attack from final stair
    REQUIRE(!attackSpots.contains(3)); // can't attack after piece is out of play
}

TEST_CASE("Validate can't attack final stair positions") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
            {
                PksColor::Yellow,
                {LAST_SHARED_SPOT - 1, LAST_SHARED_SPOT - 2, LAST_SHARED_SPOT - 4, LAST_SHARED_SPOT - 6}
            },
            {PksColor::Red, {0, 0, 0, 0}},
            {PksColor::Green, {0, 0, 0, 0}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Yellow,
    };
    auto gameSnapshot = game.start(initialBoard);

    mockDiceRoller.setNextRandomValues(1, 4);
    game.rollDice();

    auto attackSpots = game.getAttackableSpots();
    REQUIRE(!attackSpots.contains(0)); // all attackable positions would be on the stair
    REQUIRE(attackSpots[1] == std::set<int>{62});
    REQUIRE(attackSpots[2] == std::set<int>{60});
    REQUIRE(attackSpots[3] == std::set<int>{61, 62}); // 58 is a safe spot
}


TEST_CASE("Can snitch if first dice wasted") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, 0, 19, 0}},
            {PksColor::Red, {10, 15, 0, 30}},
            {PksColor::Green, {0, 0, 0, 0}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Red,
    };
    auto gameSnapshot = game.start(initialBoard);

    // As Yellow is in 19, which is Red's 2, this dice allows an attack
    mockDiceRoller.setNextRandomValues(2, 4);
    game.rollDice();

    gameSnapshot = game.getGameSnapshot();
    REQUIRE(gameSnapshot.snitchablePiece == -1); // not snitchable yet, Red can still attack

    gameSnapshot = game.useDice(2, 0);
    REQUIRE(gameSnapshot.snitchablePiece == 2); // Red lost the chance to attack, now can be snitched

    REQUIRE(!game.snitchOnPlayer(PksColor::Red, 1)); // can't snitch on wrong piece
    REQUIRE(!game.snitchOnPlayer(PksColor::Blue, 2)); // can't snitch on wrong player
    REQUIRE(game.snitchOnPlayer(PksColor::Red, 2)); // snitch successful!

    gameSnapshot = game.getGameSnapshot();
    REQUIRE(gameSnapshot.snitchablePiece == -1); // already snitched
    REQUIRE(gameSnapshot.piecesByPlayer.at(PksColor::Red) == std::vector<int>{10, 15, -1, 30}); // piece 2 back at home
}

TEST_CASE("Can snitch if second dice wasted") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, 0, 19, 0}},
            {PksColor::Red, {10, 15, 0, 30}},
            {PksColor::Green, {0, 0, 0, 0}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Red,
    };
    auto gameSnapshot = game.start(initialBoard);

    // As Yellow is in 19, which is Red's 2, this dice allows an attack
    mockDiceRoller.setNextRandomValues(2, 4);
    game.rollDice();

    gameSnapshot = game.getGameSnapshot();
    REQUIRE(gameSnapshot.snitchablePiece == -1); // not snitchable yet, Red can still attack

    gameSnapshot = game.useDice(4, 0);
    REQUIRE(gameSnapshot.snitchablePiece == -1); // not snitchable yet, Red can still attack

    gameSnapshot = game.useDice(2, 0);
    REQUIRE(gameSnapshot.snitchablePiece == 2); // Red lost the chance to attack, now can be snitched

    REQUIRE(game.snitchOnPlayer(PksColor::Red, 2)); // snitch successful!

    gameSnapshot = game.getGameSnapshot();
    REQUIRE(gameSnapshot.snitchablePiece == -1); // already snitched
    REQUIRE(gameSnapshot.piecesByPlayer.at(PksColor::Red) == std::vector<int>{10, 15, -1, 30}); // piece 2 back at home
}

TEST_CASE("Can snitch if first dice wasted and both dice were required") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, 0, 23, 0}},
            {PksColor::Red, {10, 10, 10, 0}},
            {PksColor::Green, {0, 0, 0, 0}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Red,
    };
    auto gameSnapshot = game.start(initialBoard);

    // As Yellow is in 23, which is Red's 6, this dice allows an attack but player needs to use both dice on piece 3
    mockDiceRoller.setNextRandomValues(2, 4);
    game.rollDice();

    gameSnapshot = game.getGameSnapshot();
    REQUIRE(gameSnapshot.snitchablePiece == -1); // not snitchable yet, Red can still attack

    gameSnapshot = game.useDice(4, 0);
    REQUIRE(gameSnapshot.snitchablePiece == 3); // Red lost the chance to attack, now can be snitched

    REQUIRE(game.snitchOnPlayer(PksColor::Red, 2)); // snitch successful!

    gameSnapshot = game.getGameSnapshot();
    REQUIRE(gameSnapshot.snitchablePiece == -1); // already snitched
    REQUIRE(gameSnapshot.piecesByPlayer.at(PksColor::Red) ==
            std::vector<int>{10, 10, 10, HOME_SPOT}); // piece 2 back at home
}

TEST_CASE("Can only snitch before next dice roll") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, 0, 19, 0}},
            {PksColor::Red, {10, 15, 0, 30}},
            {PksColor::Green, {0, 0, 0, 0}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Red,
    };
    auto gameSnapshot = game.start(initialBoard);

    // As Yellow is in 19, which is Red's 2, this dice allows an attack
    mockDiceRoller.setNextRandomValues(2, 4);
    game.rollDice();
    gameSnapshot = game.useDice(2, 0);
    REQUIRE(gameSnapshot.snitchablePiece == 2); // snitchable already

    gameSnapshot = game.useDice(4, 0);
    REQUIRE(gameSnapshot.snitchablePiece == 2); // still snitchable
    REQUIRE(gameSnapshot.currentPlayer == PksColor::Green); // next player's turn

    mockDiceRoller.setNextRandomValues(5, 3);
    game.rollDice();
    gameSnapshot = game.getGameSnapshot();
    REQUIRE(gameSnapshot.snitchablePiece == -1); // not snitchable anymore

    // Try to snitch too late
    REQUIRE(!game.snitchOnPlayer(PksColor::Red, 2)); // not successful
}

TEST_CASE("Validate pieces in safe spaces don't cause snitches") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, 0, 24, 0}},
            {PksColor::Red, {10, 10, 0, 10}},
            {PksColor::Green, {0, 0, 0, 0}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Red,
    };
    auto gameSnapshot = game.start(initialBoard);

    // Yellow is in 24, which is Red's 7, but it's a safe spot so not even this dice will allow an attack
    mockDiceRoller.setNextRandomValues(1, 6);
    game.rollDice();

    gameSnapshot = game.useDice(1, 0);
    REQUIRE(gameSnapshot.snitchablePiece == -1); // not snitchable
    REQUIRE(!game.snitchOnPlayer(PksColor::Red, 2)); // not successful

    gameSnapshot = game.useDice(6, 0);
    REQUIRE(gameSnapshot.snitchablePiece == -1); // not snitchable
    REQUIRE(!game.snitchOnPlayer(PksColor::Red, 2)); // not successful

    REQUIRE(gameSnapshot.currentPlayer == PksColor::Green); // next player's turn already
}

TEST_CASE("Can snitch if player captures less pieces than possible (wastes dice 1)") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    PksGameSnapshot initialBoard = {
        .piecesByPlayer = {
                {PksColor::Yellow, {20, 21, 21, 22}},
                {PksColor::Red, {0, 1, HOME_SPOT, HOME_SPOT}},
                {PksColor::Green, {0, 0, 0, 0}},
                {PksColor::Blue, {0, 0, 0, 0}},
            },
            .currentPlayer = PksColor::Red,
        };
    auto gameSnapshot = game.start(initialBoard);

    // If Red moves uses dice 1 for piece 1, it can capture 1 piece, but if it's applied to piece 2 it can capture
    // 2 pieces.
    mockDiceRoller.setNextRandomValues(3, 6);
    game.rollDice();

    gameSnapshot = game.getGameSnapshot();
    REQUIRE(gameSnapshot.snitchablePiece == -1); // not snitchable yet, Red can still attack

    gameSnapshot = game.useDice(2, 0);
    REQUIRE(gameSnapshot.snitchablePiece == 2); // Red lost the chance to attack, now can be snitched

    REQUIRE(!game.snitchOnPlayer(PksColor::Red, 1)); // can't snitch on wrong piece
    REQUIRE(!game.snitchOnPlayer(PksColor::Blue, 2)); // can't snitch on wrong player
    REQUIRE(game.snitchOnPlayer(PksColor::Red, 2)); // snitch successful!

    gameSnapshot = game.getGameSnapshot();
    REQUIRE(gameSnapshot.snitchablePiece == -1); // already snitched
    REQUIRE(gameSnapshot.piecesByPlayer.at(PksColor::Red) == std::vector<int>{10, 15, -1, 30}); // piece 2 back at home
}

//TODO: several players can snitch
*/
