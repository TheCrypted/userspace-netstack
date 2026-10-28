#include "packet_utils.hpp"

namespace netstack {

std::uint16_t internet_checksum(std::span<const std::byte> packet) {
    std::uint32_t acc = 0;

    for (std::size_t i = 0; i + 1 < packet.size(); i += 2) {
        acc += static_cast<std::uint32_t>(read_16(packet.subspan(i, 2)));
        while (acc >> 16)
            acc = (acc & 0xffff) + (acc >> 16);
    }

    if (packet.size() % 2) {
        acc += static_cast<uint16_t>(packet.back()) << 8 + 0x0000;
        while (acc >> 16)
            acc = (acc & 0xffff) + (acc >> 16);
    }

    return (~acc);
}
} // namespace netstack
