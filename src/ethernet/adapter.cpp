#include "ethernet/adapter.h"

namespace plc {

EthernetAdapter::~EthernetAdapter() {
    close();
}

bool EthernetAdapter::open(const AdapterInfo& info) {
    if (m_capture.isOpen()) {
        close();
    }

    if (!m_capture.open(info.name)) {
        return false;
    }

    m_info = info;
    return true;
}

void EthernetAdapter::close() {
    m_capture.close();
}

bool EthernetAdapter::isOpen() const {
    return m_capture.isOpen();
}

const AdapterInfo& EthernetAdapter::info() const {
    return m_info;
}

MacAddress EthernetAdapter::mac() const {
    return MacAddress();
}

} // namespace plc
