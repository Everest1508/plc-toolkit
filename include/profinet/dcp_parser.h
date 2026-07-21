#pragma once

#include "ethernet/frame.h"
#include "profinet/dcp.h"

#include <optional>

namespace plc {

class DcpParser {
public:
    static std::optional<DcpDevice> parseResponse(const EthernetFrame& frame);
    static std::vector<DcpDevice> parseResponses(const std::vector<EthernetFrame>& frames);

private:
    static void parseBlock(const uint8_t* data, std::size_t len, DcpDevice& device);
};

} // namespace plc
