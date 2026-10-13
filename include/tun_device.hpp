#ifndef NETSTACK_TUN_DEVICE_HPP
#define NETSTACK_TUN_DEVICE_HPP

#include "unique_fd.hpp"

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

#define TUN_PATH "/dev/net/tun"
#define IPV4_SIZE 65535

class TunDevice {
public:
    std::string name;
    UniqueFd tun_fd;
    std::array<std::byte, IPV4_SIZE> buffer{};
    
    explicit TunDevice(const std::string& name);
    ssize_t read_packet();
    size_t write_packet(std::span<const std::byte> packet);

    private:

};

#endif // NETSTACK_TUN_DEVICE_HPP
