#include "icmp.hpp"

namespace netstack::icmp {

EchoHeader parse_header(std::span<const std::byte> packet) {
    if (packet.size() < 8) {
        throw std::invalid_argument("ICMP packet is shorter than the minimum header size");
    }

    EchoHeader header{};
    header.type = read_8(packet.subspan(0, 1));
    header.code = read_8(packet.subspan(1, 1));
    header.checksum = read_16(packet.subspan(2, 2));
    header.identifier = read_16(packet.subspan(4, 2));
    header.seq_num = read_16(packet.subspan(6, 2));

    if (internet_checksum(packet) != 0) {
        throw std::invalid_argument("ICMP message failed checksum");
    }

    return header;
}

Packet<EchoHeader> parse_icmp_packet(std::span<const std::byte> data) {
    return {parse_header(data), data.subspan(8)};
}

} // namespace netstack::icmp
