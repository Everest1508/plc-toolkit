#include <catch2/catch_test_macros.hpp>

#include "profinet/dcp.h"
#include "profinet/dcp_parser.h"

using namespace plc;

static EthernetFrame makeIdentifyResponse(
    const MacAddress& srcMac,
    uint32_t xid,
    const ByteVec& blocks
) {
    EthernetFrame f;
    f.dst = MacAddress::fromString("00:11:22:33:44:55");
    f.dst = srcMac;
    f.src = srcMac;
    f.etherType = EtherTypeProfinetDCP;

    ByteVec payload;

    // Frame ID
    payload.push_back(0xFE);
    payload.push_back(0xFF);

    // Service ID: Identify
    payload.push_back(0x05);
    // Service Type: Response Success
    payload.push_back(0x01);

    // XID
    payload.push_back(static_cast<uint8_t>(xid >> 24));
    payload.push_back(static_cast<uint8_t>(xid >> 16));
    payload.push_back(static_cast<uint8_t>(xid >> 8));
    payload.push_back(static_cast<uint8_t>(xid & 0xFF));

    // Reserved (2 bytes)
    payload.push_back(0x00);
    payload.push_back(0x00);

    // Data Length
    uint16_t dataLen = static_cast<uint16_t>(blocks.size());
    payload.push_back(static_cast<uint8_t>(dataLen >> 8));
    payload.push_back(static_cast<uint8_t>(dataLen & 0xFF));

    // Blocks
    payload.insert(payload.end(), blocks.begin(), blocks.end());

    f.payload = std::move(payload);
    return f;
}

TEST_CASE("DcpParser ignores non-DCP frames", "[dcp][parser]") {
    EthernetFrame f;
    f.etherType = 0x0800;  // IPv4
    auto result = DcpParser::parseResponse(f);
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("DcpParser ignores non-Identify frames", "[dcp][parser]") {
    EthernetFrame f;
    f.etherType = EtherTypeProfinetDCP;
    f.payload = {0xFE, 0xFD, 0x03, 0x01, 0, 0, 0, 0, 0, 0, 0, 0};  // Get, Response
    auto result = DcpParser::parseResponse(f);
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("DcpParser parses IP block", "[dcp][parser]") {
    ByteVec blocks = {
        0x01, 0x02,  // Option: IP, Sub-option: IP Address
        0x00, 0x0C,  // Block length: 12
        0x00, 0x00,  // Block info
        // IP: 192.168.0.2
        0xC0, 0xA8, 0x00, 0x02,
        // Netmask: 255.255.255.0
        0xFF, 0xFF, 0xFF, 0x00,
        // Gateway: 192.168.0.1
        0xC0, 0xA8, 0x00, 0x01,
    };

    auto src = MacAddress::fromString("00:1e:c0:aa:bb:cc");
    auto frame = makeIdentifyResponse(src, 0x12345678, blocks);
    auto device = DcpParser::parseResponse(frame);

    REQUIRE(device.has_value());
    REQUIRE(device->ipv4 == "192.168.0.2");
    REQUIRE(device->netmask == "255.255.255.0");
    REQUIRE(device->gateway == "192.168.0.1");
    REQUIRE(device->mac == src);
    REQUIRE(device->xid == 0x12345678);
}

TEST_CASE("DcpParser parses Name of Station block", "[dcp][parser]") {
    ByteVec blocks = {
        0x02, 0x02,  // Option: Device Properties, Sub-option: Name of Station
        0x00, 0x05,  // Block length: 5
        0x00, 0x00,  // Block info
        'p', 'l', 'c', '_', '1',
    };

    auto src = MacAddress::fromString("00:1e:c0:aa:bb:cc");
    auto frame = makeIdentifyResponse(src, 0x00000001, blocks);
    auto device = DcpParser::parseResponse(frame);

    REQUIRE(device.has_value());
    REQUIRE(device->stationName == "plc_1");
}

TEST_CASE("DcpParser parses Device ID block", "[dcp][parser]") {
    ByteVec blocks = {
        0x02, 0x03,  // Option: Device Properties, Sub-option: Device ID
        0x00, 0x04,  // Block length: 4
        0x00, 0x00,  // Block info
        0x00, 0x2A,  // Vendor ID: Siemens (0x002A)
        0x03, 0x01,  // Device ID: 0x0301
    };

    auto src = MacAddress::fromString("00:1e:c0:aa:bb:cc");
    auto frame = makeIdentifyResponse(src, 0x00000002, blocks);
    auto device = DcpParser::parseResponse(frame);

    REQUIRE(device.has_value());
    REQUIRE(device->vendorId == 0x002A);
    REQUIRE(device->deviceId == 0x0301);
}

TEST_CASE("DcpParser parses Device Role block", "[dcp][parser]") {
    ByteVec blocks = {
        0x02, 0x04,  // Option: Device Properties, Sub-option: Device Role
        0x00, 0x02,  // Block length: 2
        0x00, 0x00,  // Block info
        0x01,        // Role: IO-Device
        0x00,        // Reserved
    };

    auto src = MacAddress::fromString("00:1e:c0:aa:bb:cc");
    auto frame = makeIdentifyResponse(src, 0x00000003, blocks);
    auto device = DcpParser::parseResponse(frame);

    REQUIRE(device.has_value());
    REQUIRE(device->deviceRole == 0x01);
}

TEST_CASE("DcpParser parses multiple blocks", "[dcp][parser]") {
    ByteVec blocks = {
        // IP block
        0x01, 0x02, 0x00, 0x0C, 0x00, 0x00,
        0xC0, 0xA8, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0x00, 0xC0, 0xA8, 0x00, 0x01,
        // Name of Station block (pad to even)
        0x02, 0x02, 0x00, 0x07, 0x00, 0x00,
        'e', 't', '2', '0', '0', 's', 'p', 0x00,  // padded to even
        // Device ID block
        0x02, 0x03, 0x00, 0x04, 0x00, 0x00,
        0x00, 0x2A, 0x03, 0x01,
    };

    auto src = MacAddress::fromString("00:1e:c0:aa:bb:cc");
    auto frame = makeIdentifyResponse(src, 0x00000004, blocks);
    auto device = DcpParser::parseResponse(frame);

    REQUIRE(device.has_value());
    REQUIRE(device->ipv4 == "192.168.0.2");
    REQUIRE(device->stationName == "et200sp");
    REQUIRE(device->vendorId == 0x002A);
    REQUIRE(device->deviceId == 0x0301);
}
