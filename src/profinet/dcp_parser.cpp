#include "profinet/dcp_parser.h"

#include <ws2tcpip.h>

#include <cstring>

namespace plc {

std::optional<DcpDevice> DcpParser::parseResponse(const EthernetFrame& frame) {
    if (frame.etherType != EtherTypeProfinetDCP) return std::nullopt;
    if (frame.payload.size() < 12) return std::nullopt;  // 2 frameId + 10 header minimum

    const uint8_t* data = frame.payload.data();

    // Frame ID
    uint16_t frameId = static_cast<uint16_t>(data[0] << 8 | data[1]);
    if (frameId != static_cast<uint16_t>(DcpFrameId::IdentifyRes)) return std::nullopt;

    // DCP Header
    uint8_t serviceId = data[2];
    uint8_t serviceType = data[3];
    if (serviceId != static_cast<uint8_t>(DcpServiceId::Identify)) return std::nullopt;
    if (serviceType != static_cast<uint8_t>(DcpServiceType::ResponseSuccess)) return std::nullopt;

    DcpDevice device;
    device.mac = frame.src;
    device.xid = static_cast<uint32_t>(data[4] << 24 | data[5] << 16 | data[6] << 8 | data[7]);
    // bytes 8-9: reserved
    uint16_t dataLength = static_cast<uint16_t>(data[10] << 8 | data[11]);

    // Parse blocks
    std::size_t offset = 12;
    std::size_t remaining = dataLength;

    while (remaining >= 6 && offset + 6 <= frame.payload.size()) {
        parseBlock(data + offset, remaining, device);

        uint16_t blockLen = static_cast<uint16_t>(data[offset + 2] << 8 | data[offset + 3]);
        std::size_t blockTotal = 6 + blockLen;  // header(6) + data(blockLen)
        // Pad to even boundary
        if (blockTotal % 2 != 0) blockTotal++;

        offset += blockTotal;
        if (blockTotal > remaining) break;
        remaining -= blockTotal;
    }

    return device;
}

std::vector<DcpDevice> DcpParser::parseResponses(const std::vector<EthernetFrame>& frames) {
    std::vector<DcpDevice> devices;
    for (const auto& f : frames) {
        auto dev = parseResponse(f);
        if (dev) {
            devices.push_back(std::move(*dev));
        }
    }
    return devices;
}

void DcpParser::parseBlock(const uint8_t* data, std::size_t len, DcpDevice& device) {
    if (len < 6) return;

    uint8_t option = data[0];
    uint8_t subOption = data[1];
    uint16_t blockLength = static_cast<uint16_t>(data[2] << 8 | data[3]);
    // data[4-5] = blockInfo

    const uint8_t* blockData = data + 6;
    std::size_t blockDataLen = blockLength;
    if (blockDataLen > len - 6) blockDataLen = len - 6;

    if (option == static_cast<uint8_t>(DcpOption::Ip) &&
        subOption == static_cast<uint8_t>(DcpSubIp::IpAddress) &&
        blockDataLen >= 12) {
        // IP Address block: IP(4) + Netmask(4) + Gateway(4)
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, blockData, ip, sizeof(ip));
        device.ipv4 = ip;
        inet_ntop(AF_INET, blockData + 4, ip, sizeof(ip));
        device.netmask = ip;
        inet_ntop(AF_INET, blockData + 8, ip, sizeof(ip));
        device.gateway = ip;
    }
    else if (option == static_cast<uint8_t>(DcpOption::DeviceProperties) &&
             subOption == static_cast<uint8_t>(DcpSubDevice::NameOfStation) &&
             blockDataLen > 0) {
        device.stationName.assign(reinterpret_cast<const char*>(blockData), blockDataLen);
    }
    else if (option == static_cast<uint8_t>(DcpOption::DeviceProperties) &&
             subOption == static_cast<uint8_t>(DcpSubDevice::DeviceId) &&
             blockDataLen >= 4) {
        device.vendorId = static_cast<uint16_t>(blockData[0] << 8 | blockData[1]);
        device.deviceId = static_cast<uint16_t>(blockData[2] << 8 | blockData[3]);
    }
    else if (option == static_cast<uint8_t>(DcpOption::DeviceProperties) &&
             subOption == static_cast<uint8_t>(DcpSubDevice::DeviceRole) &&
             blockDataLen >= 1) {
        device.deviceRole = blockData[0];
    }
    else if (option == static_cast<uint8_t>(DcpOption::DeviceProperties) &&
             subOption == static_cast<uint8_t>(DcpSubDevice::AliasName) &&
             blockDataLen > 0) {
        device.aliasName.assign(reinterpret_cast<const char*>(blockData), blockDataLen);
    }
    else if (option == static_cast<uint8_t>(DcpOption::DeviceProperties) &&
             subOption == static_cast<uint8_t>(DcpSubDevice::DeviceInstance) &&
             blockDataLen >= 2) {
        device.deviceInstance = static_cast<uint16_t>(blockData[0] << 8 | blockData[1]);
    }
}

} // namespace plc
