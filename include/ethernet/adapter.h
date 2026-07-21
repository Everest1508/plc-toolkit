#pragma once

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

    const AdapterInfo& info() const;
    MacAddress mac() const;

private:
    Capture m_capture;
    AdapterInfo m_info;
};

} // namespace plc
