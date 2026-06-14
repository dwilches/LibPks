#include "TestBoards.h"

TestBoard TestBoards::allPiecesAtHome() {
    return TestBoard{
        {PksColor::Yellow, {-1, -1, -1, -1}},
        {PksColor::Red, {-1, -1, -1, -1}},
        {PksColor::Green, {-1, -1, -1, -1}},
        {PksColor::Blue, {-1, -1, -1, -1}},
    };
}

TestBoard TestBoards::allPiecesAtStart() {
    return TestBoard{
        {PksColor::Yellow, {0, 0, 0, 0}},
        {PksColor::Red, {0, 0, 0, 0}},
        {PksColor::Green, {0, 0, 0, 0}},
        {PksColor::Blue, {0, 0, 0, 0}},
    };
}
