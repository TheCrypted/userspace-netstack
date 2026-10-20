#ifndef NETSTACK_TUN_DEVICE_HPP
#define NETSTACK_TUN_DEVICE_HPP

#include "unique_fd.hpp"
#include "tun_syscalls.hpp"

#include <array>
#include <cstddef>
#include <span>
#include <string>

namespace netstack {

inline constexpr char tun_path[] = "/dev/net/tun";
inline constexpr std::size_t max_ipv4_packet_size = 65'535;

class TunDevice {
public:
    std::string name;
    UniqueFd tun_fd;
    std::array<std::byte, max_ipv4_packet_size> buffer{};

    explicit TunDevice(const std::string &name, TunSyscalls &syscaller);
    std::size_t read_packet();
    std::size_t write_packet(std::span<const std::byte> packet);

private:
    TunSyscalls &_syscaller;
};

} // namespace netstack

#endif // NETSTACK_TUN_DEVICE_HPP
