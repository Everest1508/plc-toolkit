#include <catch2/catch_test_macros.hpp>

#include "ethernet/frame.h"

using namespace plc;

TEST_CASE("EthernetFrame default values", "[frame]") {
    EthernetFrame f;
    REQUIRE(f.etherType == 0);
    REQUIRE(f.payload.empty());
    REQUIRE(f.dst.isNull());
    REQUIRE(f.src.isNull());
}

TEST_CASE("EthernetFrame roundtrip via bytes", "[frame]") {
    EthernetFrame f;
    f.dst = MacAddress::fromString("ff:ff:ff:ff:ff:ff");
    f.src = MacAddress::fromString("01:02:03:04:05:06");
    f.etherType = EtherTypeProfinet;
    f.payload = {0xDE, 0xAD, 0xBE, 0xEF};

    auto raw = f.toBytes();
    REQUIRE(raw.size() == 14 + 4);

    auto f2 = EthernetFrame::fromBytes(raw);
    REQUIRE(f2.dst == f.dst);
    REQUIRE(f2.src == f.src);
    REQUIRE(f2.etherType == EtherTypeProfinet);
    REQUIRE(f2.payload == f.payload);
}

TEST_CASE("EthernetFrame fromBytes too short throws", "[frame]") {
    ByteVec short_frame = {0x00, 0x01, 0x02};
    REQUIRE_THROWS(EthernetFrame::fromBytes(short_frame));
}

TEST_CASE("EthernetFrame header layout", "[frame]") {
    ByteVec raw = {
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  // dst
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55,  // src
        0x88, 0x92,                            // EtherType PROFINET
        0x01, 0x02                             // payload
    };

    auto f = EthernetFrame::fromBytes(raw);
    REQUIRE(f.dst == MacAddress::broadcast());
    REQUIRE(f.src == MacAddress::fromString("00:11:22:33:44:55"));
    REQUIRE(f.etherType == 0x8892);
    REQUIRE(f.payload == ByteVec({0x01, 0x02}));
}

TEST_CASE("EtherType constants", "[frame]") {
    REQUIRE(EtherTypeIPv4 == 0x0800);
    REQUIRE(EtherTypeARP == 0x0806);
    REQUIRE(EtherTypeIPv6 == 0x86DD);
    REQUIRE(EtherTypeProfinet == 0x8892);
}
