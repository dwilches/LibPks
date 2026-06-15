#ifndef LIBPKS_TESTBOARDS_H
#define LIBPKS_TESTBOARDS_H

#include <map>
#include <vector>

#include "PksColor.h"

typedef std::map<PksColor, std::vector<int> > TestBoard;

namespace TestBoards {
    TestBoard allPiecesAtHome();

    TestBoard allPiecesAtStart();
};

#endif //LIBPKS_TESTBOARDS_H
