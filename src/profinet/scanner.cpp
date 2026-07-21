#include "profinet/scanner.h"
#include "profinet/dcp_builder.h"
#include "profinet/dcp_parser.h"

#include <chrono>
#include <unordered_set>

namespace plc {

ProfinetScanner::ProfinetScanner(EthernetAdapter& adapter)
    : m_adapter(adapter) {
}

std::vector<DcpDevice> ProfinetScanner::scan(int timeoutMs) {
    if (!m_adapter.isOpen()) return {};

    // Build and send DCP Identify Request
    auto req = DcpBuilder::buildIdentifyRequest(m_adapter.mac());
    if (!m_adapter.send(req)) return {};

    // Collect responses until timeout
    std::vector<EthernetFrame> responses;
    std::unordered_set<uint32_t> seenXids;

    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);

    while (std::chrono::steady_clock::now() < deadline) {
        auto frame = m_adapter.receive(100);
        if (frame.dst.isNull()) continue;

        // Filter: must be DCP Identify Response
        if (frame.etherType != EtherTypeProfinetDCP) continue;
        if (frame.payload.size() < 12) continue;

        uint16_t frameId = static_cast<uint16_t>(frame.payload[0] << 8 | frame.payload[1]);
        if (frameId != static_cast<uint16_t>(DcpFrameId::IdentifyRes)) continue;

        uint8_t serviceId = frame.payload[2];
        uint8_t serviceType = frame.payload[3];
        if (serviceId != static_cast<uint8_t>(DcpServiceId::Identify)) continue;
        if (serviceType != static_cast<uint8_t>(DcpServiceType::ResponseSuccess)) continue;

        responses.push_back(std::move(frame));
    }

    return DcpParser::parseResponses(responses);
}

} // namespace plc
