#ifndef NETSTACK_TUN_DEVICE_HPP
#define NETSTACK_TUN_DEVICE_HPP

#include "unique_fd.hpp"

#include <array>
#include <cerrno>
#include <iostream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <cstddef>
#include <span>
#include <stdexcept>
#include <system_error>

namespace netstack {

inline constexpr char tun_path[] = "/dev/net/tun";
inline constexpr std::size_t max_ipv4_packet_size = 65'535;

class TunDevice {
public:
    std::string name;
    UniqueFd tun_fd;
    std::array<std::byte, max_ipv4_packet_size> buffer{};
    
    explicit TunDevice(const std::string& name);
    ssize_t read_packet();
    size_t write_packet(std::span<const std::byte> packet);

    private:

};

} // namespace netstack

#endif // NETSTACK_TUN_DEVICE_HPP
