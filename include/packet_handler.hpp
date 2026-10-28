#ifndef NETSTACK_PACKET_HANDLER_HPP
#define NETSTACK_PACKET_HANDLER_HPP

#include <cstddef>
#include <span>

namespace netstack {

template <typename HeaderType> struct Packet {
    HeaderType header{};
    std::span<const std::byte> data;
};

// Extract ICMP headers and payload from ip packet payload
void handle_icmp_packet(std::span<const std::byte> data);

// Extract relevant fields and pass on packet to appropriate protocol handler
void handle_packet(std::span<const std::byte> data);

} // namespace netstack

#endif // NETSTACK_PACKET_HANDLER_HPP
