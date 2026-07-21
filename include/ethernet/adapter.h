#pragma once

#include "common/types.h"
#include "ethernet/frame.h"
#include "ethernet/mac.h"
#include "network/capture.h"

#include <string>

namespace plc {

class EthernetAdapter {
public:
    EthernetAdapter() = default;
    ~EthernetAdapter();

    EthernetAdapter(const EthernetAdapter&) = delete;
    EthernetAdapter& operator=(const EthernetAdapter&) = delete;

    bool open(const AdapterInfo& info);
    void close();
    bool isOpen() const;

    bool send(const EthernetFrame& frame);
    EthernetFrame receive(int timeoutMs = 1000);

    const AdapterInfo& info() const;
    MacAddress mac() const;

private:
    Capture m_capture;
    AdapterInfo m_info;
};

} // namespace plc
