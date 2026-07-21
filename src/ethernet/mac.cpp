#include "ethernet/mac.h"

#include <charconv>
#include <stdexcept>

namespace plc {

MacAddress::MacAddress(std::string_view str) {
    *this = fromString(str);
}

MacAddress MacAddress::fromString(std::string_view str) {
    if (str.size() != 17) {
        throw std::invalid_argument("MAC address must be 17 characters (xx:xx:xx:xx:xx:xx)");
    }

    std::array<uint8_t, 6> bytes{};
    for (int i = 0; i < 6; ++i) {
        if (i < 5 && str[i * 3 + 2] != ':') {
            throw std::invalid_argument("MAC address separator must be ':'");
        }
        unsigned int val = 0;
        auto [ptr, ec] = std::from_chars(str.data() + i * 3,
                                         str.data() + i * 3 + 2,
                                         val, 16);
        if (ec != std::errc{}) {
            throw std::invalid_argument("Invalid hex in MAC address");
        }
        bytes[i] = static_cast<uint8_t>(val);
    }

    MacAddress m;
    m.m_bytes = bytes;
    return m;
}

const std::array<uint8_t, 6>& MacAddress::bytes() const {
    return m_bytes;
}

std::string MacAddress::toString() const {
    char buf[18];
    std::snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
                  m_bytes[0], m_bytes[1], m_bytes[2],
                  m_bytes[3], m_bytes[4], m_bytes[5]);
    return std::string(buf);
}

bool MacAddress::isNull() const {
    static constexpr std::array<uint8_t, 6> zero{};
    return m_bytes == zero;
}

bool MacAddress::isBroadcast() const {
    static constexpr std::array<uint8_t, 6> bcast = {
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff
    };
    return m_bytes == bcast;
}

bool MacAddress::operator==(const MacAddress& other) const {
    return m_bytes == other.m_bytes;
}

bool MacAddress::operator!=(const MacAddress& other) const {
    return m_bytes != other.m_bytes;
}

bool MacAddress::operator<(const MacAddress& other) const {
    return m_bytes < other.m_bytes;
}

MacAddress MacAddress::broadcast() {
    return MacAddress("ff:ff:ff:ff:ff:ff");
}

} // namespace plc
