
enable_testing()

add_executable(libpks_tests
        src/TestBoards.cpp
        test_dice_roller.cpp
        test_piece_movement.cpp
        test_piece_capture.cpp
        test_convert_spot_numbers.cpp
        test_game_over.cpp
)

target_link_libraries(libpks_tests PRIVATE libpks)

add_test(NAME AllTests COMMAND libpks_tests)

# Install Catch2, for running automated tests: https://github.com/catchorg/Catch2
# <editor-fold desc="Configure Catch2">
include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.2  # Use the latest version tag
)
FetchContent_MakeAvailable(Catch2)

target_link_libraries(libpks_tests PRIVATE Catch2::Catch2WithMain)
# </editor-fold>

