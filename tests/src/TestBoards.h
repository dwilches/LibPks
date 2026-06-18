#ifndef LIBPKS_TESTBOARDS_H
#define LIBPKS_TESTBOARDS_H

#include "PksGameSnapshot.h"

#define ALL_AT_START std::vector{0,0,0,0}
#define ALL_AT_HOME std::vector{-1,-1,-1,-1}

namespace TestBoards {
    PksPiecesByPlayer allPiecesAtHome();

    PksPiecesByPlayer allPiecesAtStart();
};

#endif //LIBPKS_TESTBOARDS_H
