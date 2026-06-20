#include <iostream>

#include "PksColor.h"
#include "PksGameState.h"
#include "PksMoves.h"

std::ostream &operator<<(std::ostream &os, const PksColor &color) {
    switch (color) {
        case PksColor::Red: os << "Red";
            break;
        case PksColor::Green: os << "Green";
            break;
        case PksColor::Blue: os << "Blue";
            break;
        case PksColor::Yellow: os << "Yellow";
            break;
        default: os << "Unknown";
            break;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const PksGameState &color) {
    switch (color) {
        case PksGameState::GameInCourse: os << "GameInCourse";
            break;
        case PksGameState::GameNotStarted: os << "GameNotStarted";
            break;
        case PksGameState::GameOver: os << "GameOver";
            break;
        default: os << "Unknown";
            break;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const PksSMove &move) {
    os << "{P=" << move.pieceIdx
            << ", D=" << move.diceValue
            << ", N=" << move.numCaptured
            << "}";
    return os;
}
