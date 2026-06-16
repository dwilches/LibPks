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
    auto gameState = game.start(initialBoard);
    REQUIRE(gameState.currentPlayer == PksColor::Yellow);

    // Roll a dice and go on the final stair
    mockDiceRoller.setNextRandomValues(6, 6);
    game.rollDice();

    gameState = game.useDice(6, 0);
    REQUIRE(gameState.piecesByPlayer[PksColor::Yellow] == std::vector{66, 0, 0, 0});

    // Use the second dice to go to the game target
    gameState = game.useDice(6, 0);
    REQUIRE(gameState.piecesByPlayer[PksColor::Yellow] == std::vector{FINAL_TARGET_SPOT, 0, 0, 0});

    // The player can roll again, as it was doubles
    REQUIRE(gameState.currentPlayer == PksColor::Yellow);
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
    auto gameState = game.start(initialBoard);
    REQUIRE(gameState.currentPlayer == PksColor::Yellow);

    // Roll a dice and go on the final stair
    mockDiceRoller.setNextRandomValues(5, 2);
    game.rollDice();

    gameState = game.useDice(2, 0);
    REQUIRE(gameState.piecesByPlayer[PksColor::Yellow] == std::vector{FINAL_TARGET_SPOT, 0, 0, 0});

    // Attempt to use the other dice to move a piece that is out of play
    REQUIRE_THROWS(game.useDice(5, 0));

    // The player will need to move a different piece
    REQUIRE(gameState.currentPlayer == PksColor::Yellow);

    gameState = game.useDice(5, 1);
    REQUIRE(gameState.piecesByPlayer[PksColor::Yellow] == std::vector{FINAL_TARGET_SPOT, 5, 0, 0});
    REQUIRE(gameState.currentPlayer == PksColor::Red); // next player's turn
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
    auto gameState = game.start(initialBoard);
    REQUIRE(gameState.currentPlayer == PksColor::Yellow);

    // Roll a dice and go on the final stair
    mockDiceRoller.setNextRandomValues(3, 4);
    game.rollDice();
    gameState = game.useDice(3, 0);

    REQUIRE(gameState.piecesByPlayer[PksColor::Yellow] ==
            std::vector{FINAL_TARGET_SPOT, 70, FINAL_TARGET_SPOT, FINAL_TARGET_SPOT});
    REQUIRE(gameState.gameState == PksGameState::GameInCourse);

    // Move the remaining piece
    gameState = game.useDice(4, 1);

    REQUIRE(gameState.piecesByPlayer[PksColor::Yellow] == std::vector(4, FINAL_TARGET_SPOT));
    REQUIRE(gameState.gameState == PksGameState::GameOver);
}
