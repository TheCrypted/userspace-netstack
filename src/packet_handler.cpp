#include "packet_handler.hpp"
#include "ipv4.hpp"
#include "packet_utils.hpp"

#include <stdexcept>

namespace netstack {

void handle_packet(std::span<const std::byte> data) {
    auto ip_packet = ipv4::parse_ip_packet(data);
    switch (ip_packet.header.protocol) {
    case 1:
        handle_icmp_packet(ip_packet.data);
        break;
    default:
        throw std::invalid_argument("Failed to recognise packet protocol");
    }
}

} // namespace netstack
