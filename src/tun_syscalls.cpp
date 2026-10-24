
#include <fcntl.h>
#include "tun_syscalls.hpp"
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace netstack {

int LinuxTunSyscalls::open_device() {
    return ::open("/dev/net/tun", O_RDWR | O_CLOEXEC);
}

int LinuxTunSyscalls::close(int fd) noexcept {
    return ::close(fd);
}

int LinuxTunSyscalls::attach(int fd, ifreq &request) {
    return ::ioctl(fd, TUNSETIFF, &request);
}

ssize_t LinuxTunSyscalls::read(int fd, std::span<std::byte> buffer) {
    return ::read(fd, buffer.data(), buffer.size());
}

ssize_t LinuxTunSyscalls::write(int fd, std::span<const std::byte> packet) {
    return ::write(fd, packet.data(), packet.size());
}

} // namespace netstack
