#include <catch2/catch_test_macros.hpp>

#include "MockDiceRoller.h"
#include "PksConstants.h"
#include "PksGame.h"
#include "PksUtils.h"
#include "src/TestBoards.h"

TEST_CASE("A piece can reach the final stair") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    PksBoardState initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {60, 0, 0, 0}},
            {PksColor::Red, {0, 0, 0, 0}},
            {PksColor::Green, {0, 0, 0, 0}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Yellow,
    };
    REQUIRE(game.start(initialBoard) == PksColor::Yellow);

    // Roll a dice and go on the final stair
    mockDiceRoller.setNextRandomValues(6, 6);
    game.rollDice();
    game.useDice(6, 0);

    auto currentState = game.getCurrentBoardState();
    REQUIRE(currentState.piecesByPlayer[PksColor::Yellow] == std::vector{66, 0, 0, 0});

    // Use the second dice to go to the game target
    game.useDice(6, 0);
    currentState = game.getCurrentBoardState();
    REQUIRE(currentState.piecesByPlayer[PksColor::Yellow] == std::vector{FINAL_TARGET_SPOT, 0, 0, 0});

    // The player can roll again, as it was doubles
    REQUIRE(currentState.currentPlayer == PksColor::Yellow);
}

TEST_CASE("A piece cannot move after reaching the final target") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    PksBoardState initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {70, 0, 0, 0}},
            {PksColor::Red, {0, 0, 0, 0}},
            {PksColor::Green, {0, 0, 0, 0}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Yellow,
    };
    REQUIRE(game.start(initialBoard) == PksColor::Yellow);

    // Roll a dice and go on the final stair
    mockDiceRoller.setNextRandomValues(5, 2);
    game.rollDice();
    game.useDice(2, 0);

    auto currentState = game.getCurrentBoardState();
    REQUIRE(currentState.piecesByPlayer[PksColor::Yellow] == std::vector{FINAL_TARGET_SPOT, 0, 0, 0});

    // Attempt to use the other dice to move a piece that is out of play
    REQUIRE_THROWS(game.useDice(5, 0));

    // The player will need to move a different piece
    REQUIRE(currentState.currentPlayer == PksColor::Yellow);
    game.useDice(5, 1);

    currentState = game.getCurrentBoardState();
    REQUIRE(currentState.piecesByPlayer[PksColor::Yellow] == std::vector{FINAL_TARGET_SPOT, 5, 0, 0});
    REQUIRE(currentState.currentPlayer == PksColor::Red); // next player's turn
}

TEST_CASE("Game ends after all pieces reach the final spot") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    PksBoardState initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {70, 70, FINAL_TARGET_SPOT, FINAL_TARGET_SPOT}},
            {PksColor::Red, {0, 0, 0, 0}},
            {PksColor::Green, {0, 0, 0, 0}},
            {PksColor::Blue, {0, 0, 0, 0}},
        },
        .currentPlayer = PksColor::Yellow,
    };
    REQUIRE(game.start(initialBoard) == PksColor::Yellow);

    // Roll a dice and go on the final stair
    mockDiceRoller.setNextRandomValues(3, 4);
    game.rollDice();
    game.useDice(3, 0);

    auto currentState = game.getCurrentBoardState();
    REQUIRE(currentState.piecesByPlayer[PksColor::Yellow] ==
            std::vector{FINAL_TARGET_SPOT, 70, FINAL_TARGET_SPOT, FINAL_TARGET_SPOT});
    REQUIRE(currentState.gameState == PksGameState::GameInCourse);

    // Move the remaining piece
    game.useDice(4, 1);

    currentState = game.getCurrentBoardState();
    REQUIRE(currentState.piecesByPlayer[PksColor::Yellow] == std::vector(4, FINAL_TARGET_SPOT));
    REQUIRE(currentState.gameState == PksGameState::GameOver);
}
