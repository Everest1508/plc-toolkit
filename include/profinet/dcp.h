#pragma once

#include "ethernet/mac.h"

#include <cstdint>
#include <string>
#include <vector>

namespace plc {

// Ethernet
constexpr uint16_t EtherTypeProfinetDCP = 0x8892;

// PROFINET DCP Multicast MAC
constexpr MacAddress DcpMulticastMac = MacAddress::fromBytes({0x01, 0x0e, 0xcf, 0x00, 0x00, 0x00});

// Frame IDs
enum class DcpFrameId : uint16_t {
    Hello        = 0xFEFC,
    GetSet       = 0xFEFD,
    IdentifyReq  = 0xFEFE,
    IdentifyRes  = 0xFEFF,
};

// Service IDs
enum class DcpServiceId : uint8_t {
    Reserved      = 0x00,
    Manufacturer1 = 0x01,
    Manufacturer2 = 0x02,
    Get           = 0x03,
    Set           = 0x04,
    Identify      = 0x05,
    Hello         = 0x06,
};

// Service Types
enum class DcpServiceType : uint8_t {
    Request            = 0x00,
    ResponseSuccess    = 0x01,
    ResponseUnsupported = 0x05,
};

// DCP Options
enum class DcpOption : uint8_t {
    Ip                 = 0x01,
    DeviceProperties   = 0x02,
    Dhcp               = 0x03,
    Control            = 0x05,
    DeviceInitiative   = 0x06,
    All                = 0xFF,
};

// DCP Sub-options — IP (Option 0x01)
enum class DcpSubIp : uint8_t {
    Mac       = 0x01,
    IpAddress = 0x02,
    FullIp    = 0x03,
};

// DCP Sub-options — Device Properties (Option 0x02)
enum class DcpSubDevice : uint8_t {
    ManufacturerSpecific = 0x01,
    NameOfStation        = 0x02,
    DeviceId             = 0x03,
    DeviceRole           = 0x04,
    DeviceOptions        = 0x05,
    AliasName            = 0x06,
    DeviceInstance       = 0x07,
    OemId                = 0x08,
};

// DCP Header (after PROFINET IO frame ID)
#pragma pack(push, 1)
struct DcpHeader {
    uint8_t  serviceId;
    uint8_t  serviceType;
    uint32_t xid;
    uint16_t responseDelay;  // Identify Request: response delay ×10ms
    uint16_t dataLength;     // total length of following blocks
};
#pragma pack(pop)

// DCP Block (TLV inside the data area)
#pragma pack(push, 1)
struct DcpBlockHeader {
    uint8_t  option;
    uint8_t  subOption;
    uint16_t blockLength;   // length of data after this header
    uint16_t blockInfo;
};
#pragma pack(pop)

// Parsed device from DCP Identify Response
struct DcpDevice {
    MacAddress mac;
    std::string stationName;
    std::string aliasName;
    std::string ipv4;
    std::string netmask;
    std::string gateway;
    uint16_t vendorId = 0;
    uint16_t deviceId = 0;
    uint8_t deviceRole = 0;
    uint16_t deviceInstance = 0;
    uint32_t xid = 0;
};

} // namespace plc
