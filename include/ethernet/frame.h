#pragma once

#include "common/types.h"
#include "ethernet/mac.h"

#include <cstdint>

namespace plc {

constexpr uint16_t EtherTypeIPv4 = 0x0800;
constexpr uint16_t EtherTypeARP  = 0x0806;
constexpr uint16_t EtherTypeIPv6 = 0x86DD;
constexpr uint16_t EtherTypeProfinet = 0x8892;

struct EthernetFrame {
    MacAddress dst;
    MacAddress src;
    uint16_t etherType = 0;
    ByteVec payload;

    static EthernetFrame fromBytes(const ByteVec& raw);
    ByteVec toBytes() const;
};

} // namespace plc
