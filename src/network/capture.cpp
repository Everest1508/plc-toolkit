#include "network/capture.h"

#include <ws2tcpip.h>

#include <stdexcept>

namespace plc {

Capture::~Capture() {
    close();
}

Capture::Capture(Capture&& other) noexcept
    : m_handle(other.m_handle) {
    other.m_handle = nullptr;
}

Capture& Capture::operator=(Capture&& other) noexcept {
    if (this != &other) {
        close();
        m_handle = other.m_handle;
        other.m_handle = nullptr;
    }
    return *this;
}

std::vector<AdapterInfo> Capture::enumerate() {
    pcap_if_t* alldevs = nullptr;
    char errbuf[PCAP_ERRBUF_SIZE];

    if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, nullptr, &alldevs, errbuf) == -1) {
        throw std::runtime_error(std::string("pcap_findalldevs_ex failed: ") + errbuf);
    }

    std::vector<AdapterInfo> adapters;
    for (pcap_if_t* d = alldevs; d != nullptr; d = d->next) {
        AdapterInfo info;
        info.name = d->name ? d->name : "";
        info.description = d->description ? d->description : "";
        info.isLoopback = (d->flags & PCAP_IF_LOOPBACK) != 0;
        info.isUp = (d->flags & PCAP_IF_UP) != 0;

        for (pcap_addr_t* addr = d->addresses; addr != nullptr; addr = addr->next) {
            if (!addr->addr) continue;

            if (addr->addr->sa_family == AF_INET) {
                auto* sin = reinterpret_cast<struct sockaddr_in*>(addr->addr);
                char ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &sin->sin_addr, ip, sizeof(ip));
                info.ipv4 = ip;
            } else if (addr->addr->sa_family == AF_INET6) {
                auto* sin6 = reinterpret_cast<struct sockaddr_in6*>(addr->addr);
                char ip6[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, &sin6->sin6_addr, ip6, sizeof(ip6));
                info.ipv6 = ip6;
            }
        }

        adapters.push_back(std::move(info));
    }

    pcap_freealldevs(alldevs);
    return adapters;
}

bool Capture::open(const std::string& deviceName) {
    if (m_handle) {
        close();
    }

    char errbuf[PCAP_ERRBUF_SIZE];
    m_handle = pcap_open_live(deviceName.c_str(), 65535, 1, 1000, errbuf);
    return m_handle != nullptr;
}

void Capture::close() {
    if (m_handle) {
        pcap_close(m_handle);
        m_handle = nullptr;
    }
}

bool Capture::isOpen() const {
    return m_handle != nullptr;
}

pcap_t* Capture::handle() const {
    return m_handle;
}

} // namespace plc
