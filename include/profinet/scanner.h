#pragma once

#include "ethernet/adapter.h"
#include "profinet/dcp.h"

#include <vector>

namespace plc {

class ProfinetScanner {
public:
    explicit ProfinetScanner(EthernetAdapter& adapter);
    std::vector<DcpDevice> scan(int timeoutMs = 3000);

private:
    EthernetAdapter& m_adapter;
};

} // namespace plc
