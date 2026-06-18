
add_library(libpks STATIC
        PksGame.cpp
        PksDiceRoller.cpp
        PksException.cpp
        PksPlayer.cpp
        PksColor.cpp
        PksUtils.cpp
        PksGameState.cpp
        PksDiceResult.cpp
        PksSnitchablePieces.cpp
        include/PksSnitchablePieces.h
        include/PksTypeDefs.h
        PksGameBoard.cpp
        PksGameBoard.h
)

target_include_directories(libpks
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(libpks PRIVATE -Wall -Wextra -pedantic)
