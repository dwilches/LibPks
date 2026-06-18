#include "PksGameBoard.h"

#include "PksConstants.h"
#include "PksException.h"
#include "PksUtils.h"

int PksGameBoard::movePiece(PksPiecesByPlayer &board,
                            const PksColor currentPlayer,
                            const PIECE_IDX pieceIdx,
                            const DICE_VAL diceVal) {
    auto &playerPieces = board.at(currentPlayer);
    if (playerPieces[pieceIdx] == FINAL_TARGET_SPOT || playerPieces[pieceIdx] == HOME_SPOT) {
        throw PksException{
            "PksGameBoard::movePiece(): Attempted to move a piece that is out of play: " + std::to_string(pieceIdx)
        };
    }

    // Move the player's piece to the target spot
    const SPOT_NUM targetSpot = std::min(playerPieces[pieceIdx] + diceVal, FINAL_TARGET_SPOT);
    playerPieces[pieceIdx] = targetSpot;

    // If the spot is not shared unsafe, there is nothign we can capture
    if (PksUtils::getSpotType(targetSpot) != PksSpotType::UnsafeShared) {
        return 0;
    }

    // Capture any pieces that are at the target spot
    int numCaptured = 0;
    for (auto &[otherPlayerColor, otherPlayerPieces]: board) {
        if (otherPlayerColor == currentPlayer) {
            continue;
        }

        const auto &otherSpot = PksUtils::convertSpotNumber(currentPlayer, otherPlayerColor, targetSpot);

        // If the other player has any piece at the target spot, move them back home
        for (auto &otherPlayerPiece: otherPlayerPieces) {
            if (otherPlayerPiece == otherSpot) {
                otherPlayerPiece = HOME_SPOT;
                numCaptured++;
            }
        }
    }
    return numCaptured;
}
