#include <catch2/catch_test_macros.hpp>

#include "MockDiceRoller.h"
#include "PksGame.h"
#include "src/TestBoards.h"

TEST_CASE("All pieces move out of home") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    // The current player is not at Home
    PksBoardState initialBoard = {
        .piecesByPlayer = TestBoards::allPiecesAtHome(),
        .currentPlayer = PksColor::Yellow,
    };
    REQUIRE(game.start(initialBoard) == PksColor::Yellow);
    REQUIRE(game.getCurrentBoardState().piecesByPlayer[PksColor::Yellow] == std::vector{-1, -1, -1, -1}); // all at home

    // Roll doubles to get out of home
    mockDiceRoller.setNextRandomValues(6, 6);
    auto diceRoll = game.rollDice();
    REQUIRE(diceRoll.first == RolledDice{.value=6, .alreadyUsed=true}); // used to get out of home
    REQUIRE(diceRoll.second == RolledDice{.value=6, .alreadyUsed=true});
    REQUIRE(game.getCurrentBoardState().piecesByPlayer[PksColor::Yellow] == std::vector{0, 0, 0, 0}); // all at start

    // Next player's turn
    REQUIRE(game.getCurrentBoardState().currentPlayer == PksColor::Red);

    // Next player is still at home
    REQUIRE(game.getCurrentBoardState().piecesByPlayer[PksColor::Red] == std::vector{-1, -1, -1, -1});
}

TEST_CASE("Pieces move according to dice value (same piece moves both dice)") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    // The current player is not at Home
    PksBoardState initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, 10, 20, 30}},
            {PksColor::Red, {-1, -1, -1, -1}},
            {PksColor::Green, {-1, -1, -1, -1}},
            {PksColor::Blue, {-1, -1, -1, -1}},
        },
        .currentPlayer = PksColor::Yellow,
    };
    REQUIRE(game.start(initialBoard) == PksColor::Yellow);
    REQUIRE(game.getCurrentBoardState().piecesByPlayer[PksColor::Yellow] == std::vector{0, 10, 20, 30});

    // Roll a dice and ensure the pieces move the desired amounts
    mockDiceRoller.setNextRandomValues(6, 3);
    auto diceRoll = game.rollDice();
    REQUIRE(game.getCurrentBoardState().piecesByPlayer[PksColor::Yellow] == std::vector{0, 10, 20, 30});

    game.useDice(6, 1); // Should move from 10 to 16
    REQUIRE(game.getCurrentBoardState().piecesByPlayer[PksColor::Yellow] == std::vector{0, 16, 20, 30});

    game.useDice(3, 1); // Should move from 16 to 19
    REQUIRE(game.getCurrentBoardState().piecesByPlayer[PksColor::Yellow] == std::vector{0, 19, 20, 30});

    // Next player's turn
    REQUIRE(game.getCurrentBoardState().currentPlayer == PksColor::Red);
}

TEST_CASE("Pieces move according to dice value (different pieces move)") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    // The current player is not at Home
    PksBoardState initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, 10, 20, 30}},
            {PksColor::Red, {-1, -1, -1, -1}},
            {PksColor::Green, {-1, -1, -1, -1}},
            {PksColor::Blue, {-1, -1, -1, -1}},
        },
        .currentPlayer = PksColor::Yellow,
    };
    REQUIRE(game.start(initialBoard) == PksColor::Yellow);
    REQUIRE(game.getCurrentBoardState().piecesByPlayer[PksColor::Yellow] == std::vector{0, 10, 20, 30});

    // Roll a dice and ensure the pieces move the desired amounts
    mockDiceRoller.setNextRandomValues(6, 3);
    auto diceRoll = game.rollDice();
    REQUIRE(game.getCurrentBoardState().piecesByPlayer[PksColor::Yellow] == std::vector{0, 10, 20, 30});

    game.useDice(6, 2); // Should move from 20 to 26
    REQUIRE(game.getCurrentBoardState().piecesByPlayer[PksColor::Yellow] == std::vector{0, 10, 26, 30});

    game.useDice(3, 1); // Should move from 10 to 13
    REQUIRE(game.getCurrentBoardState().piecesByPlayer[PksColor::Yellow] == std::vector{0, 13, 26, 30});

    // Next player's turn
    REQUIRE(game.getCurrentBoardState().currentPlayer == PksColor::Red);
}

TEST_CASE("Pieces move according to dice value (doubles test)") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    // The current player is not at Home
    PksBoardState initialBoard = {
        .piecesByPlayer = {
            {PksColor::Yellow, {0, 10, 20, 30}},
            {PksColor::Red, {-1, -1, -1, -1}},
            {PksColor::Green, {-1, -1, -1, -1}},
            {PksColor::Blue, {-1, -1, -1, -1}},
        },
        .currentPlayer = PksColor::Yellow,
    };
    REQUIRE(game.start(initialBoard) == PksColor::Yellow);
    REQUIRE(game.getCurrentBoardState().piecesByPlayer[PksColor::Yellow] == std::vector{0, 10, 20, 30});

    // Roll a dice and ensure the pieces move the desired amounts
    mockDiceRoller.setNextRandomValues(6, 6);
    game.rollDice();
    game.useDice(6, 0);
    game.useDice(6, 1);
    REQUIRE(game.getCurrentBoardState().piecesByPlayer[PksColor::Yellow] == std::vector{6, 16, 20, 30});

    // Same player's turn, as it was doubles
    REQUIRE(game.getCurrentBoardState().currentPlayer == PksColor::Yellow);

    // Roll another dice
    mockDiceRoller.setNextRandomValues(5, 5);
    game.rollDice();
    game.useDice(5, 1);
    game.useDice(5, 2);
    REQUIRE(game.getCurrentBoardState().piecesByPlayer[PksColor::Yellow] == std::vector{6, 21, 25, 30});

    // Still, same player's turn, as it was doubles
    REQUIRE(game.getCurrentBoardState().currentPlayer == PksColor::Yellow);

    // Roll another dice
    mockDiceRoller.setNextRandomValues(2, 2);
    game.rollDice();
    game.useDice(2, 1);
    game.useDice(2, 0);
    REQUIRE(game.getCurrentBoardState().piecesByPlayer[PksColor::Yellow] == std::vector{8, 23, 25, 30});

    // Too many doubles, next player's turn
    REQUIRE(game.getCurrentBoardState().currentPlayer == PksColor::Red);
}

TEST_CASE("Pieces move according to dice value (all players move)") {
    MockDiceRoller mockDiceRoller;
    PksGame game{mockDiceRoller};

    // All pieces are at the start
    PksBoardState initialBoard = {
        .piecesByPlayer = TestBoards::allPiecesAtStart(),
        .currentPlayer = PksColor::Yellow,
    };
    REQUIRE(game.start(initialBoard) == PksColor::Yellow);

    // Roll a dice and ensure the pieces move the desired amounts
    mockDiceRoller.setNextRandomValues(1, 2);
    game.rollDice();
    game.useDice(1, 0);
    game.useDice(2, 1);
    REQUIRE(game.getCurrentBoardState().piecesByPlayer[PksColor::Yellow] == std::vector{1, 2, 0, 0});

    // Next player rolls a dice
    mockDiceRoller.setNextRandomValues(5, 3);
    game.rollDice();
    game.useDice(5, 1);
    game.useDice(3, 2);
    REQUIRE(game.getCurrentBoardState().piecesByPlayer[PksColor::Red] == std::vector{0, 5, 3, 0});

    // Next player rolls a dice (doubles)
    mockDiceRoller.setNextRandomValues(3, 3);
    game.rollDice();
    game.useDice(3, 0);
    game.useDice(3, 1);
    REQUIRE(game.getCurrentBoardState().piecesByPlayer[PksColor::Green] == std::vector{3, 3, 0, 0});

    // Same player can roll again
    mockDiceRoller.setNextRandomValues(2, 3);
    game.rollDice();
    game.useDice(2, 0);
    game.useDice(3, 1);
    REQUIRE(game.getCurrentBoardState().piecesByPlayer[PksColor::Green] == std::vector{5, 6, 0, 0});

    // Last player rolls a dice
    mockDiceRoller.setNextRandomValues(6, 3);
    game.rollDice();
    game.useDice(3, 3);
    game.useDice(6, 3);
    REQUIRE(game.getCurrentBoardState().piecesByPlayer[PksColor::Blue] == std::vector{0, 0, 0, 9});

    // The next player would be the first one
    REQUIRE(game.getCurrentBoardState().currentPlayer == PksColor::Yellow);

    // Ensure the whole board is as we expect
    auto finalBoard = game.getCurrentBoardState().piecesByPlayer;
    REQUIRE(finalBoard[PksColor::Yellow] == std::vector{1, 2, 0, 0});
    REQUIRE(finalBoard[PksColor::Red] == std::vector{0, 5, 3, 0});
    REQUIRE(finalBoard[PksColor::Green] == std::vector{5, 6, 0, 0});
    REQUIRE(finalBoard[PksColor::Blue] == std::vector{0, 0, 0, 9});
}
