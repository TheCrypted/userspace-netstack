#ifndef NETSTACK_PACKET_UTILS_HPP
#define NETSTACK_PACKET_UTILS_HPP

#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>

namespace netstack {

constexpr std::uint8_t read_8(std::span<const std::byte> byte_arr) {
    if (byte_arr.size() != 1) {
        throw std::invalid_argument("read_8() received incompatible byte array");
    }

    return std::to_integer<std::uint8_t>(byte_arr[0]);
}

constexpr std::uint16_t read_16(std::span<const std::byte> byte_arr) {
    if (byte_arr.size() != 2) {
        throw std::invalid_argument("read_16() received incompatible byte array");
    }

    std::uint16_t res = 0;
    res += std::to_integer<unsigned>(byte_arr[0]) << 8;
    res |= std::to_integer<unsigned>(byte_arr[1]);

    return res;
}

constexpr std::uint32_t read_32(std::span<const std::byte> byte_arr) {
    if (byte_arr.size() != 4) {
        throw std::invalid_argument("read_32() received incompatible byte array");
    }

    return static_cast<std::uint32_t>((std::to_integer<unsigned>(byte_arr[0]) << 24) |
                                      (std::to_integer<unsigned>(byte_arr[1]) << 16) |
                                      (std::to_integer<unsigned>(byte_arr[2]) << 8) |
                                      std::to_integer<unsigned>(byte_arr[3]));
}

constexpr void write_16(std::span<std::byte> byte_arr, std::uint16_t value) {
    if (byte_arr.size() != 2) {
        throw std::invalid_argument("write_16() received incompatible byte array");
    }

    byte_arr[0] = static_cast<std::byte>((value >> 8) & 0xff);
    byte_arr[1] = static_cast<std::byte>(value & 0xff);
}

constexpr void write_32(std::span<std::byte> byte_arr, std::uint32_t value) {
    if (byte_arr.size() != 4) {
        throw std::invalid_argument("write_32() received incompatible byte array");
    }

    byte_arr[0] = static_cast<std::byte>((value >> 24) & 0xff);
    byte_arr[1] = static_cast<std::byte>((value >> 16) & 0xff);
    byte_arr[2] = static_cast<std::byte>((value >> 8) & 0xff);
    byte_arr[3] = static_cast<std::byte>(value & 0xff);
}

std::uint16_t internet_checksum(std::span<const std::byte> packet);

} // namespace netstack

#endif // NETSTACK_PACKET_UTILS_HPP
