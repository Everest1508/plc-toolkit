#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

namespace plc {

class MacAddress {
public:
    MacAddress() = default;
    explicit MacAddress(std::string_view str);

    static constexpr MacAddress fromBytes(const std::array<uint8_t, 6>& bytes) {
        MacAddress m;
        m.m_bytes = bytes;
        return m;
    }
    static MacAddress fromString(std::string_view str);

    const std::array<uint8_t, 6>& bytes() const;
    std::string toString() const;
    bool isNull() const;
    bool isBroadcast() const;

    bool operator==(const MacAddress& other) const;
    bool operator!=(const MacAddress& other) const;
    bool operator<(const MacAddress& other) const;

    static MacAddress broadcast();

private:
    std::array<uint8_t, 6> m_bytes{};
};

} // namespace plc
