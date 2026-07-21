#pragma once

#include <pcap.h>

#include <string>
#include <vector>

namespace plc {

struct AdapterInfo {
    std::string name;
    std::string description;
    std::string ipv4;
    std::string ipv6;
    bool isUp = false;
    bool isLoopback = false;
};

class Capture {
public:
    Capture() = default;
    ~Capture();

    Capture(const Capture&) = delete;
    Capture& operator=(const Capture&) = delete;
    Capture(Capture&& other) noexcept;
    Capture& operator=(Capture&& other) noexcept;

    std::vector<AdapterInfo> enumerate();
    bool open(const std::string& deviceName);
    void close();
    bool isOpen() const;
    pcap_t* handle() const;

private:
    pcap_t* m_handle = nullptr;
};

} // namespace plc
