#pragma once

#include <cstdint>
#include <vector>

namespace plc {

using ByteVec = std::vector<uint8_t>;

enum class Error {
    None,
    Timeout,
    Io,
    InvalidParam,
    NotOpen,
};

} // namespace plc
