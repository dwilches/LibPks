#include <catch2/catch_test_macros.hpp>

#include "PksConstants.h"
#include "PksUtils.h"

TEST_CASE("Conversion of spots numbers (from Yellow)") {
    // Start position
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Yellow, PksColor::Red, 0) == 51);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Yellow, PksColor::Green, 0) == 34);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Yellow, PksColor::Blue, 0) == 17);

    // Last shared position
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Yellow, PksColor::Red, LAST_SHARED_SPOT) == 46);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Yellow, PksColor::Green, LAST_SHARED_SPOT) == 29);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Yellow, PksColor::Blue, LAST_SHARED_SPOT) == 12);

    // Some intermediate positions (hitting edge cases in the implementation)
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Yellow, PksColor::Red, 17) == 0);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Yellow, PksColor::Green, 17) == 51);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Yellow, PksColor::Blue, 17) == 34);

    REQUIRE(PksUtils::convertSpotNumber(PksColor::Yellow, PksColor::Red, 51) == 34);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Yellow, PksColor::Green, 51) == 17);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Yellow, PksColor::Blue, 51) == 0);
}

TEST_CASE("Conversion of spots numbers (from Red)") {
    // Start position
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Red, PksColor::Yellow, 0) == 17);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Red, PksColor::Green, 0) == 51);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Red, PksColor::Blue, 0) == 34);

    // Last shared position
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Red, PksColor::Yellow, LAST_SHARED_SPOT) == 12);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Red, PksColor::Green, LAST_SHARED_SPOT) == 46);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Red, PksColor::Blue, LAST_SHARED_SPOT) == 29);

    // Some intermediate positions (hitting edge cases in the implementation)
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Red, PksColor::Yellow, 17) == 34);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Red, PksColor::Green, 17) == 0);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Red, PksColor::Blue, 17) == 51);

    REQUIRE(PksUtils::convertSpotNumber(PksColor::Red, PksColor::Yellow, 51) == 0);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Red, PksColor::Green, 51) == 34);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Red, PksColor::Blue, 51) == 17);
}

TEST_CASE("Conversion of spots numbers (from Green)") {
    // Start position
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Green, PksColor::Yellow, 0) == 34);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Green, PksColor::Red, 0) == 17);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Green, PksColor::Blue, 0) == 51);

    // Last shared position
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Green, PksColor::Yellow, LAST_SHARED_SPOT) == 29);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Green, PksColor::Red, LAST_SHARED_SPOT) == 12);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Green, PksColor::Blue, LAST_SHARED_SPOT) == 46);

    // Some intermediate positions (hitting edge cases in the implementation)
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Green, PksColor::Yellow, 17) == 51);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Green, PksColor::Red, 17) == 34);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Green, PksColor::Blue, 17) == 0);

    REQUIRE(PksUtils::convertSpotNumber(PksColor::Green, PksColor::Yellow, 51) == 17);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Green, PksColor::Red, 51) == 0);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Green, PksColor::Blue, 51) == 34);
}

TEST_CASE("Conversion of spots numbers (from Blue)") {
    // Start position
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Blue, PksColor::Yellow, 0) == 51);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Blue, PksColor::Red, 0) == 34);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Blue, PksColor::Green, 0) == 17);

    // Last shared position
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Blue, PksColor::Yellow, LAST_SHARED_SPOT) == 46);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Blue, PksColor::Red, LAST_SHARED_SPOT) == 29);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Blue, PksColor::Green, LAST_SHARED_SPOT) == 12);

    // Some intermediate positions (hitting edge cases in the implementation)
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Blue, PksColor::Yellow, 17) == 0);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Blue, PksColor::Red, 17) == 51);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Blue, PksColor::Green, 17) == 34);

    REQUIRE(PksUtils::convertSpotNumber(PksColor::Blue, PksColor::Yellow, 51) == 34);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Blue, PksColor::Red, 51) == 17);
    REQUIRE(PksUtils::convertSpotNumber(PksColor::Blue, PksColor::Green, 51) == 0);
}

TEST_CASE("Validate before converting spots numbers") {
    // can't convert from a color to itself
    REQUIRE_THROWS(PksUtils::convertSpotNumber(PksColor::Yellow, PksColor::Yellow, 0));

    // Can't convert invalid spot numbers
    REQUIRE_THROWS(PksUtils::convertSpotNumber(PksColor::Yellow, PksColor::Yellow, -1));
    REQUIRE_THROWS(PksUtils::convertSpotNumber(PksColor::Yellow, PksColor::Yellow, LAST_SHARED_SPOT + 1));
}
