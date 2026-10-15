#ifndef NETSTACK_IPV4_HPP
#define NETSTACK_IPV4_HPP

#include <cstddef>
#include <span>

namespace netstack::ipv4 {

constexpr unsigned version(std::span<const std::byte> packet) {
    auto ver_nibble = std::to_integer<unsigned>(packet[0] >> 4);
    return ver_nibble;
}


} // namespace netstack::ipv4

#endif // NETSTACK_IPV4_HPP
