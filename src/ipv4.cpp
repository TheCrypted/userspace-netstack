#include "ipv4.hpp"

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

// Compares the supplied checksum against the header, skipping checksum bytes 10-11.
bool checksum(std::span<const std::byte> packet, std::uint16_t header_checksum, std::uint8_t ihl) {
    const auto header_length = static_cast<std::size_t>(ihl) * 4;
    if (ihl < 5 || ihl > 15 || header_length > packet.size()) {
        throw std::invalid_argument("Invalid IPv4 header length for checksum");
    }

    std::uint32_t acc = 0;

    for (std::size_t i = 0; i < header_length; i += 2) {
        if (i == 10)
            continue; // the checksum field itself
        acc += static_cast<std::uint32_t>(read_16(packet.subspan(i, 2)));
        while (acc >> 16)
            acc = (acc & 0xffff) + (acc >> 16);
    }

    return static_cast<std::uint16_t>(~acc) == header_checksum;
}

} // namespace netstack::ipv4
