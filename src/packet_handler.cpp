#include "packet_handler.hpp"
#include "ipv4.hpp"
#include "icmp.hpp"
#include "packet_utils.hpp"

#include <stdexcept>

namespace netstack {

void handle_icmp_packet(std::span<std::byte> data) {
    auto icmp_packet = icmp::parse_icmp_packet(data);
    icmp::create_reply(data, icmp_packet.header, icmp_packet.data);
}

void handle_packet(std::span<std::byte> data) {
    auto ip_packet = ipv4::parse_ip_packet(data);
    auto protocol = static_cast<Protocol>(ip_packet.header.protocol);

    switch (protocol) {
    case Protocol::Icmp:
        handle_icmp_packet(ip_packet.data);
        break;
    default:
        throw std::invalid_argument("Failed to recognise packet protocol");
    }
}

} // namespace netstack
