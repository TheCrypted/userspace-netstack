#ifndef NETSTACK_TUN_SYSCALLS_HPP
#define NETSTACK_TUN_SYSCALLS_HPP

#include <cstddef>
#include <linux/if.h>
#include <span>
#include <sys/types.h>

namespace netstack {

class TunSyscalls {
public:
    virtual ~TunSyscalls() = default;

    virtual int open_device() = 0;
    virtual int attach(int fd, ifreq &request) = 0;
    virtual ssize_t read(int fd, std::span<std::byte> buffer) = 0;
    virtual ssize_t write(int fd, std::span<const std::byte> packet) = 0;
};

class LinuxTunSyscalls final : public TunSyscalls {
public:
    int open_device() override;
    int attach(int fd, ifreq &request) override;
    ssize_t read(int fd, std::span<std::byte> buffer) override;
    ssize_t write(int fd, std::span<const std::byte> packet) override;
};

} // namespace netstack

#endif // NETSTACK_TUN_SYSCALLS_HPP
