
add_library(libpks STATIC
        PksGame.cpp
        PksDiceRoller.cpp
        PksException.cpp
        PksPlayer.cpp
        PksUtils.cpp
        PksDiceResult.cpp
        PksSnitcher.cpp
        PksGameBoard.cpp
        PksLogUtils.cpp
)

target_include_directories(libpks
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(libpks PRIVATE -Wall -Wextra -pedantic)
