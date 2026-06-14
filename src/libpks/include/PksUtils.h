#ifndef LIBPKS_PKSUTILS_H
#define LIBPKS_PKSUTILS_H
#include "PksColor.h"


class PksUtils {
public:
    PksUtils() = delete;

    static int convertSpotNumber(const PksColor &from, const PksColor &to, int spot);
};


#endif //LIBPKS_PKSUTILS_H
