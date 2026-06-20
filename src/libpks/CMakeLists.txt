
add_library(libpks STATIC
        PksGame.cpp
        PksGameBoard.cpp
        PksDiceRoller.cpp
        PksException.cpp
        PksUtils.cpp
        PksDiceResult.cpp
        PksSnitcher.cpp
        PksLogUtils.cpp
)

target_include_directories(libpks
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(libpks PRIVATE -Wall -Wextra -pedantic)
