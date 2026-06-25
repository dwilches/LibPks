
set(SOURCES
        PksGame.cpp
        PksGameBoard.cpp
        PksDiceRoller.cpp
        PksException.cpp
        PksUtils.cpp
        PksDiceResult.cpp
        PksSnitcher.cpp
        PksLogUtils.cpp
        CBindings.cpp)

# Generate a static library
add_library(libpks STATIC ${SOURCES})
target_include_directories(libpks PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(libpks PRIVATE -Wall -Wextra -pedantic)
set_target_properties(libpks PROPERTIES OUTPUT_NAME "pks")

# And also a dynamic library
add_library(libpks_dll SHARED ${SOURCES})
target_include_directories(libpks_dll PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(libpks_dll PRIVATE -Wall -Wextra -pedantic)
set_target_properties(libpks_dll PROPERTIES OUTPUT_NAME "pks")
