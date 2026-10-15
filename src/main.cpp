#include "tun_device.hpp"
#include "ipv4.hpp"

#include <iostream>


int main() {
    std::cout << "Application started || Reading stream" << std::endl;
    auto td = netstack::TunDevice("tun0");

    while (true) {
        try {
            auto packet_size = td.read_packet();
            if (packet_size > 0) {
                auto packet = std::span<const std::byte>{
                    td.buffer.data(), static_cast<std::size_t>(packet_size)};
                std::cout << "Received packet: " << packet_size
                          << " bytes, IP version " << netstack::ipv4::version(packet)
                          << std::endl;
            }
        } catch (const std::runtime_error& error) {
            std::cerr << error.what() << '\n';
        }
    }
    
    return 0;
}
