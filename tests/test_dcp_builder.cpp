#include <catch2/catch_test_macros.hpp>

#include "profinet/dcp.h"
#include "profinet/dcp_builder.h"
#include "profinet/dcp_parser.h"

using namespace plc;

TEST_CASE("DCP Identify Request frame structure", "[dcp][builder]") {
    auto src = MacAddress::fromString("01:02:03:04:05:06");
    auto frame = DcpBuilder::buildIdentifyRequest(src);

    SECTION("Ethernet header") {
        REQUIRE(frame.dst == DcpMulticastMac);
        REQUIRE(frame.src == src);
        REQUIRE(frame.etherType == EtherTypeProfinetDCP);
    }

    SECTION("minimum frame size") {
        auto raw = frame.toBytes();
        REQUIRE(raw.size() >= 60);
    }

    SECTION("PROFINET IO frame ID") {
        REQUIRE(frame.payload.size() >= 2);
        uint16_t frameId = static_cast<uint16_t>(frame.payload[0] << 8 | frame.payload[1]);
        REQUIRE(frameId == 0xFEFE);
    }

    SECTION("DCP header") {
        REQUIRE(frame.payload[2] == 0x05);  // serviceId: Identify
        REQUIRE(frame.payload[3] == 0x00);  // serviceType: Request
    }

    SECTION("DCP data length") {
        uint16_t dataLen = static_cast<uint16_t>(frame.payload[10] << 8 | frame.payload[11]);
        REQUIRE(dataLen == 0x0004);
    }

    SECTION("selector block") {
        REQUIRE(frame.payload[12] == 0xFF);  // option: ALL
        REQUIRE(frame.payload[13] == 0xFF);  // sub-option: ALL
        uint16_t blockLen = static_cast<uint16_t>(frame.payload[14] << 8 | frame.payload[15]);
        REQUIRE(blockLen == 0x0000);
    }
}

TEST_CASE("DCP constants", "[dcp]") {
    REQUIRE(static_cast<uint16_t>(DcpFrameId::IdentifyReq) == 0xFEFE);
    REQUIRE(static_cast<uint16_t>(DcpFrameId::IdentifyRes) == 0xFEFF);
    REQUIRE(static_cast<uint8_t>(DcpServiceId::Identify) == 0x05);
    REQUIRE(static_cast<uint8_t>(DcpServiceType::Request) == 0x00);
    REQUIRE(static_cast<uint8_t>(DcpServiceType::ResponseSuccess) == 0x01);
    REQUIRE(EtherTypeProfinetDCP == 0x8892);
}

TEST_CASE("DcpHeader layout", "[dcp]") {
    REQUIRE(sizeof(DcpHeader) == 10);
}

TEST_CASE("DcpBlockHeader layout", "[dcp]") {
    REQUIRE(sizeof(DcpBlockHeader) == 6);
}
