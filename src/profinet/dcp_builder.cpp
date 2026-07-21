#include "profinet/dcp_builder.h"
#include "profinet/dcp.h"

#include <cstring>
#include <random>

namespace plc {

// Minimum Ethernet frame size (without CRC)
static constexpr std::size_t kMinFrameSize = 60;

EthernetFrame DcpBuilder::buildIdentifyRequest(const MacAddress& srcMac) {
    EthernetFrame frame;
    frame.dst = DcpMulticastMac;
    frame.src = srcMac;
    frame.etherType = EtherTypeProfinetDCP;

    // Build payload manually
    ByteVec payload;
    payload.reserve(26);  // 2 + 10 + 4 + pad

    // PROFINET IO Frame ID
    uint16_t frameId = static_cast<uint16_t>(DcpFrameId::IdentifyReq);
    payload.push_back(static_cast<uint8_t>(frameId >> 8));
    payload.push_back(static_cast<uint8_t>(frameId & 0xFF));

    // DCP Header
    payload.push_back(static_cast<uint8_t>(DcpServiceId::Identify));  // serviceId
    payload.push_back(static_cast<uint8_t>(DcpServiceType::Request)); // serviceType

    // XID (transaction ID)
    uint32_t xid = randomXid();
    payload.push_back(static_cast<uint8_t>(xid >> 24));
    payload.push_back(static_cast<uint8_t>(xid >> 16));
    payload.push_back(static_cast<uint8_t>(xid >> 8));
    payload.push_back(static_cast<uint8_t>(xid & 0xFF));

    // Response delay
    uint16_t delay = randomResponseDelay();
    payload.push_back(static_cast<uint8_t>(delay >> 8));
    payload.push_back(static_cast<uint8_t>(delay & 0xFF));

    // DCP Data Length (4 = selector block)
    payload.push_back(0x00);
    payload.push_back(0x04);

    // Selector block: Option=ALL, SubOption=ALL, BlockLength=0
    payload.push_back(0xFF);  // option
    payload.push_back(0xFF);  // sub-option
    payload.push_back(0x00);  // block length high
    payload.push_back(0x00);  // block length low

    frame.payload = std::move(payload);

    // Pad to minimum Ethernet frame size
    // Ethernet header (14) + payload must be >= 60
    auto raw = frame.toBytes();
    if (raw.size() < kMinFrameSize) {
        raw.resize(kMinFrameSize, 0x00);
        frame = EthernetFrame::fromBytes(raw);
    }

    return frame;
}

uint32_t DcpBuilder::randomXid() {
    static std::mt19937 rng(std::random_device{}());
    return rng();
}

uint16_t DcpBuilder::randomResponseDelay() {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<uint16_t> dist(0, 9);
    return dist(rng);
}

} // namespace plc
