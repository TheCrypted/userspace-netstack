#ifndef NETSTACK_IPV4_HPP
#define NETSTACK_IPV4_HPP

#include "packet_handler.hpp"

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

Header parse_header(std::span<const std::byte> packet);
Packet<Header> parse_ip_packet(std::span<const std::byte> data);

} // namespace netstack::ipv4

#endif // NETSTACK_IPV4_HPP
