#include "tun_device.hpp"
#include "ipv4.hpp"
#include "tun_syscalls.hpp"

#include <cstddef>
#include <iostream>
#include <span>
#include <stdexcept>

int main() {
    std::cout << "Application started || Reading stream" << std::endl;
    netstack::LinuxTunSyscalls syscalls;
    auto td = netstack::TunDevice("tun0", syscalls);

    while (true) {
        try {
            auto packet_size = td.read_packet();
            if (packet_size > 0) {
                auto packet = std::span<const std::byte>{td.buffer.data(), packet_size};
                std::cout << "Received packet: " << packet_size << " bytes, IP version "
                          << static_cast<unsigned>(netstack::ipv4::version(packet)) << std::endl;
            }
        } catch (const std::runtime_error &error) {
            std::cerr << error.what() << '\n';
        }
    }

    return 0;
}
