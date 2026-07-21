#pragma once

#include "ethernet/frame.h"
#include "ethernet/mac.h"

namespace plc {

class DcpBuilder {
public:
    static EthernetFrame buildIdentifyRequest(const MacAddress& srcMac);

private:
    static uint32_t randomXid();
    static uint16_t randomResponseDelay();
};

} // namespace plc
