#ifndef NETSTACK_UNIQUE_FD_HPP
#define NETSTACK_UNIQUE_FD_HPP

#include "tun_syscalls.hpp"

#include <unistd.h>

namespace netstack {

class UniqueFd {
public:
    explicit UniqueFd(int fd = -1) noexcept : fd(fd) {}
    // The syscall provider must outlive this descriptor and any moved owner.
    UniqueFd(int fd, TunSyscalls &syscalls) noexcept : fd(fd), syscalls(&syscalls) {}
    ~UniqueFd() {
        if (fd >= 0)
            close_descriptor();
    }

    // Prevent copy construction
    UniqueFd(const UniqueFd &) = delete;
    // Prevent copy assignment
    UniqueFd &operator=(const UniqueFd &) = delete;

    UniqueFd(UniqueFd &&other) noexcept : fd(other.fd), syscalls(other.syscalls) { other.fd = -1; }
    UniqueFd &operator=(UniqueFd &&other) noexcept {
        if (this == &other)
            return *this;
        if (fd >= 0) {
            close_descriptor();
        }

        fd = other.fd;
        syscalls = other.syscalls;
        other.fd = -1;
        return *this;
    }

    int get() const noexcept { return fd; }

    bool is_valid() { return fd >= 0; }

private:
    void close_descriptor() noexcept {
        if (syscalls)
            syscalls->close(fd);
        else
            ::close(fd);
    }

    int fd;
    TunSyscalls *syscalls = nullptr;
};

} // namespace netstack

#endif // NETSTACK_UNIQUE_FD_HPP
