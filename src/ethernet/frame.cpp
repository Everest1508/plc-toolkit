#include "ethernet/frame.h"

#include <stdexcept>

namespace plc {

EthernetFrame EthernetFrame::fromBytes(const ByteVec& raw) {
    if (raw.size() < 14) {
        throw std::invalid_argument("Ethernet frame too short (minimum 14 bytes)");
    }

    EthernetFrame f;
    f.dst = MacAddress::fromBytes({raw[0], raw[1], raw[2], raw[3], raw[4], raw[5]});
    f.src = MacAddress::fromBytes({raw[6], raw[7], raw[8], raw[9], raw[10], raw[11]});
    f.etherType = static_cast<uint16_t>(raw[12] << 8 | raw[13]);
    f.payload.assign(raw.begin() + 14, raw.end());
    return f;
}

ByteVec EthernetFrame::toBytes() const {
    ByteVec raw;
    raw.reserve(14 + payload.size());

    const auto& dstBytes = dst.bytes();
    const auto& srcBytes = src.bytes();
    raw.insert(raw.end(), dstBytes.begin(), dstBytes.end());
    raw.insert(raw.end(), srcBytes.begin(), srcBytes.end());
    raw.push_back(static_cast<uint8_t>(etherType >> 8));
    raw.push_back(static_cast<uint8_t>(etherType & 0xFF));
    raw.insert(raw.end(), payload.begin(), payload.end());

    return raw;
}

} // namespace plc
