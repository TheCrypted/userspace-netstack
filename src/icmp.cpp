#include "icmp.hpp"

#include <algorithm>

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

Packet<EchoHeader> parse_icmp_packet(std::span<std::byte> data) {
    return {parse_header(data), data.subspan(8)};
}

void create_header(std::span<std::byte> buffer, std::uint8_t type, std::uint16_t identifier,
                   std::uint16_t seq_num, size_t payload_len) {
    if (buffer.size() < 8) {
        throw std::invalid_argument("ICMP packet is shorter than the minimum header size");
    }

    write_8(buffer.subspan(0, 1), type);
    write_8(buffer.subspan(1, 1), 0);  // Code
    write_16(buffer.subspan(2, 2), 0); // put a 0 value in for checksum initially
    write_16(buffer.subspan(4, 2), identifier);
    write_16(buffer.subspan(6, 2), seq_num);

    auto checksum = internet_checksum(buffer.first(8 + payload_len));
    write_16(buffer.subspan(2, 2), checksum);
}

void create_reply(std::span<std::byte> buffer, const EchoHeader &header,
                  std::span<const std::byte> data) {
    size_t header_size = 8;
    if (data.size() + header_size > buffer.size())
        throw std::invalid_argument("Reply buffer incorrect size for payload");

    std::copy(data.begin(), data.end(), buffer.begin() + header_size);

    create_header(buffer, 0, header.identifier, header.seq_num, data.size());
}

} // namespace netstack::icmp
