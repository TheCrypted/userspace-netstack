#ifndef NETSTACK_ICMP_HPP
#define NETSTACK_ICMP_HPP

#include "packet_utils.hpp"
#include "packet_handler.hpp"

#include <cstdint>
#include <stdexcept>
#include <span>

namespace netstack::icmp {

struct EchoHeader {
    std::uint8_t type{};
    std::uint8_t code{};
    std::uint16_t checksum{};
    std::uint16_t identifier{};
    std::uint16_t seq_num{};
};

EchoHeader parse_header(std::span<const std::byte> packet);

Packet<EchoHeader> parse_icmp_packet(std::span<const std::byte> data);

} // namespace netstack::icmp

#endif // NETSTACK_ICMP_HPP
