#ifndef LIBPKS_PKSCOLOR_H
#define LIBPKS_PKSCOLOR_H

#include <iosfwd>

enum class PksColor: int {
    Yellow, Red, Green, Blue
};

// Useful for debugging
std::ostream &operator<<(std::ostream &, const PksColor &);

#endif //LIBPKS_PKSCOLOR_H
