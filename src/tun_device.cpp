#include "tun_device.hpp"

namespace netstack {

TunDevice::TunDevice(const std::string& name) : name(name), tun_fd(open(tun_path, O_RDWR)) {
    if (!tun_fd.is_valid()) {
        throw std::system_error(errno, std::generic_category(), "Failed to open TUN device");
    }

    struct ifreq ifr{};
    std::strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ - 1);
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI; 

    if (ioctl(tun_fd.get(), TUNSETIFF, &ifr) == -1) {
        throw std::system_error(errno, std::generic_category(), "TUNSETIFF failed");
    }
}

ssize_t TunDevice::read_packet() {
    while (true) {
        auto packet_size = read(tun_fd.get(), buffer.data(), buffer.size());
        if (packet_size < 0) {
            const int error = errno;
            if (error == EINTR) // We were interrupted during read syscall; retry
                continue;
            throw std::system_error(error, std::generic_category(), "TUN read failed");
        }
        return packet_size;
    }

}

size_t TunDevice::write_packet(std::span<const std::byte> packet) {
    if (packet.size() > max_ipv4_packet_size) {
        throw std::invalid_argument("write_packet received packet greater than allowed size.");
    } else if (packet.empty()) {
        throw std::invalid_argument("write_packet called with empty packet.");
    }

    while (true) {
        auto size = write(tun_fd.get(), packet.data(), packet.size());
        if (size == -1) {
            const int error = errno;
            if (error == EINTR)
                continue;
            throw std::system_error(error, std::generic_category(), "TUN write failed");
        }

        const auto written = static_cast<std::size_t>(size);
        if (written != packet.size()) {
            throw std::runtime_error("Short TUN packet write");
        }
        return written;
    }
}

} // namespace netstack
