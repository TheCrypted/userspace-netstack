#include <tun_device.hpp>
#include <stdexcept>

TunDevice::TunDevice(const std::string& name) : name(name), tun_fd(open(TUN_PATH, O_RDWR)) {
    if (!tun_fd.is_valid()) {
        // open failed
        throw std::runtime_error("Failed to open TUN device");
    }

    struct ifreq ifr{};
    std::strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ - 1);
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI; 

    if (ioctl(tun_fd.get(), TUNSETIFF, &ifr) == -1) {
        throw std::runtime_error("Failed to establish ioctl to tun0");
    }
}

ssize_t TunDevice::read_packet() {
    while (true) {
        auto packet_size = read(tun_fd.get(), &buffer, buffer.size());
        if (packet_size < 0) {
            if (errno == EINTR) // We were interrupted during read syscall; retry
                continue;
            throw std::runtime_error("Read failed on tun");
        } {
            return packet_size;
        }
    }

}

size_t TunDevice::write_packet(std::span<const std::byte> packet) {
    return 1;
}
