#ifndef LIBPKS_PKSGAMESTATE_H
#define LIBPKS_PKSGAMESTATE_H

#include <iosfwd>

enum class PksGameState {
    GameNotStarted, GameInCourse, GameOver
};

std::ostream &operator<<(std::ostream &, const PksGameState &);

#endif //LIBPKS_PKSGAMESTATE_H
