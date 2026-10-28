#include "ipv4.hpp"
#include "packet_utils.hpp"

namespace netstack::ipv4 {

Header parse_header(std::span<const std::byte> packet) {
    if (packet.size() < 20) {
        throw std::invalid_argument("IPv4 packet is shorter than the minimum header size");
    }

    Header header;
    header.version = version(packet);
    header.ihl = ihl(packet);
    if (header.ihl < 5) {
        throw std::invalid_argument("IPv4 Header has IHL below 5");
    }

    const auto header_length = static_cast<std::size_t>(header.ihl) * 4;
    if (header_length > packet.size()) {
        throw std::invalid_argument("IPv4 header length exceeds received packet size");
    }

    header.tos = read_8(packet.subspan(1, 1));
    header.tot_length = read_16(packet.subspan(2, 2));
    if (header.tot_length < header_length) {
        throw std::invalid_argument("IPv4 total length is smaller than header length");
    }
    if (header.tot_length > packet.size()) {
        throw std::invalid_argument("IPv4 total length exceeds received packet size");
    }
    header.identification = read_16(packet.subspan(4, 2));

    const auto flags_and_offset = read_16(packet.subspan(6, 2));
    header.flags = static_cast<std::uint8_t>(flags_and_offset >> 13);
    header.fragment_offset = flags_and_offset & 0x1fff;

    header.ttl = read_8(packet.subspan(8, 1));
    header.protocol = read_8(packet.subspan(9, 1));
    header.checksum = read_16(packet.subspan(10, 2));
    header.source_addr = read_32(packet.subspan(12, 4));
    header.dest_addr = read_32(packet.subspan(16, 4));

    return header;
}

Packet<Header> parse_ip_packet(std::span<const std::byte> data) {
    Packet<Header> packet{parse_header(data), {}};
    const auto header_len = static_cast<std::size_t>(packet.header.ihl) * 4;
    if (internet_checksum(data.first(header_len)) != 0)
        throw std::invalid_argument("Provided packet contained invalid header");

    packet.data = data.subspan(header_len, packet.header.tot_length - header_len);

    return packet;
}

} // namespace netstack::ipv4
