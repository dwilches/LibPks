#include <iostream>

#include "PksGameState.h"

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
