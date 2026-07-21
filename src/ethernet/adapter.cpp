#include "ethernet/adapter.h"

#include <iphlpapi.h>
#include <windows.h>

#include <vector>

namespace plc {

namespace {

std::string extractGuid(const std::string& deviceName) {
    auto start = deviceName.find('{');
    auto end = deviceName.find('}');
    if (start == std::string::npos || end == std::string::npos || end <= start) {
        return {};
    }
    return deviceName.substr(start, end - start + 1);
}

} // anonymous namespace

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

bool EthernetAdapter::send(const EthernetFrame& frame) {
    if (!m_capture.isOpen()) return false;

    auto raw = frame.toBytes();
    return pcap_sendpacket(m_capture.handle(), raw.data(), static_cast<int>(raw.size())) == 0;
}

EthernetFrame EthernetAdapter::receive(int timeoutMs) {
    pcap_t* h = m_capture.handle();
    if (!h) return {};

    struct pcap_pkthdr* header = nullptr;
    const u_char* pktData = nullptr;

    int res = pcap_next_ex(h, &header, &pktData);
    if (res <= 0 || header->caplen < 14) {
        return {};
    }

    ByteVec raw(pktData, pktData + header->caplen);
    return EthernetFrame::fromBytes(raw);
}

const AdapterInfo& EthernetAdapter::info() const {
    return m_info;
}

MacAddress EthernetAdapter::mac() const {
    std::string guid = extractGuid(m_info.name);
    if (guid.empty()) return MacAddress();

    ULONG bufLen = 15000;
    std::vector<BYTE> buf(bufLen);
    auto* addrs = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buf.data());

    DWORD ret = GetAdaptersAddresses(AF_UNSPEC,
        GAA_FLAG_SKIP_UNICAST | GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST,
        nullptr, addrs, &bufLen);

    if (ret != NO_ERROR) return MacAddress();

    for (auto* a = addrs; a; a = a->Next) {
        if (guid == a->AdapterName && a->PhysicalAddressLength == 6) {
            std::array<uint8_t, 6> macBytes;
            memcpy(macBytes.data(), a->PhysicalAddress, 6);
            return MacAddress::fromBytes(macBytes);
        }
    }
    return MacAddress();
}

} // namespace plc
