#include "TestBoards.h"

PksPiecesByPlayer TestBoards::allPiecesAtHome() {
    return {
        {PksColor::Yellow, {-1, -1, -1, -1}},
        {PksColor::Red, {-1, -1, -1, -1}},
        {PksColor::Green, {-1, -1, -1, -1}},
        {PksColor::Blue, {-1, -1, -1, -1}},
    };
}

PksPiecesByPlayer TestBoards::allPiecesAtStart() {
    return {
        {PksColor::Yellow, {0, 0, 0, 0}},
        {PksColor::Red, {0, 0, 0, 0}},
        {PksColor::Green, {0, 0, 0, 0}},
        {PksColor::Blue, {0, 0, 0, 0}},
    };
}
