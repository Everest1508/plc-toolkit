#include <catch2/catch_test_macros.hpp>

#include "ethernet/mac.h"

using namespace plc;

TEST_CASE("MacAddress default is null", "[mac]") {
    MacAddress m;
    REQUIRE(m.isNull());
}

TEST_CASE("MacAddress from string", "[mac]") {
    auto m = MacAddress::fromString("01:23:45:67:89:ab");
    REQUIRE(m.toString() == "01:23:45:67:89:ab");
    REQUIRE_FALSE(m.isNull());
    REQUIRE_FALSE(m.isBroadcast());
}

TEST_CASE("MacAddress broadcast", "[mac]") {
    auto b = MacAddress::broadcast();
    REQUIRE(b.isBroadcast());
    REQUIRE(b.toString() == "ff:ff:ff:ff:ff:ff");
}

TEST_CASE("MacAddress from bytes", "[mac]") {
    std::array<uint8_t, 6> bytes = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    auto m = MacAddress::fromBytes(bytes);
    REQUIRE(m.bytes() == bytes);
    REQUIRE(m.toString() == "aa:bb:cc:dd:ee:ff");
}

TEST_CASE("MacAddress comparison", "[mac]") {
    auto a = MacAddress::fromString("00:00:00:00:00:01");
    auto b = MacAddress::fromString("00:00:00:00:00:02");
    REQUIRE(a != b);
    REQUIRE(a < b);
    REQUIRE_FALSE(a == b);
}

TEST_CASE("MacAddress equality", "[mac]") {
    auto a = MacAddress::fromString("aa:bb:cc:dd:ee:ff");
    auto b = MacAddress::fromString("aa:bb:cc:dd:ee:ff");
    REQUIRE(a == b);
}

TEST_CASE("MacAddress invalid string throws", "[mac]") {
    REQUIRE_THROWS(MacAddress::fromString("not-a-mac"));
    REQUIRE_THROWS(MacAddress::fromString("00:00:00:00:00"));
    REQUIRE_THROWS(MacAddress::fromString("00:00:00:00:00:00:00"));
}

TEST_CASE("MacAddress constructor from string", "[mac]") {
    MacAddress m("01:02:03:04:05:06");
    REQUIRE(m.toString() == "01:02:03:04:05:06");
}
