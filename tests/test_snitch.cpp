#include <catch2/catch_test_macros.hpp>

#include "MockDiceRoller.h"
#include "PksConstants.h"
#include "PksGame.h"
#include "src/TestBoards.h"

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

//TODO:
TEST_CASE("Validate pieces can be snitched") {

}

TEST_CASE("Validate pieces in safe spaces don't cause snitches") {

}

TEST_CASE("Validate snitches expire after the next roll") {

}
