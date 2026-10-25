#ifndef NETSTACK_IPV4_HPP
#define NETSTACK_IPV4_HPP

#include <cstddef>
#include <stdexcept>
#include <span>
#include <cstdint>

namespace netstack::ipv4 {

struct Header {
    std::uint8_t version{};
    std::uint8_t ihl{}; // In 32-bit words.
    std::uint8_t tos{};
    std::uint16_t tot_length{};
    std::uint16_t identification{};
    std::uint8_t flags{};
    std::uint16_t fragment_offset{}; // In eight-byte units.
    std::uint8_t ttl{};
    std::uint8_t protocol{};
    std::uint16_t checksum{};
    std::uint32_t source_addr{};
    std::uint32_t dest_addr{};
};

constexpr std::uint8_t version(std::span<const std::byte> packet) {
    if (packet.empty()) {
        throw std::invalid_argument("version requested for empty packet");
    }

    auto ver_nibble = std::to_integer<std::uint8_t>(packet[0] >> 4);
    return ver_nibble;
}

constexpr std::uint8_t ihl(std::span<const std::byte> packet) {
    if (packet.empty()) {
        throw std::invalid_argument("ihl requested for empty packet");
    }

    return std::to_integer<std::uint8_t>(packet[0] & std::byte{0x0f});
}

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

Header parse_header(std::span<const std::byte> packet);
bool checksum(std::span<const std::byte> packet, std::uint16_t header_checksum, std::uint8_t ihl);

} // namespace netstack::ipv4

#endif // NETSTACK_IPV4_HPP
