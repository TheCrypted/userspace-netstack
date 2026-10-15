#ifndef NETSTACK_UNIQUE_FD_HPP
#define NETSTACK_UNIQUE_FD_HPP

#include <unistd.h>

namespace netstack {

class UniqueFd {
public:
    explicit UniqueFd(int fd = -1) noexcept : fd(fd) {}
    ~UniqueFd() {
        if (fd >= 0)
            close(fd);
    }

    // Prevent copy construction
    UniqueFd(const UniqueFd&) = delete;
    // Prevent copy assignment
    UniqueFd& operator=(const UniqueFd&) = delete;

    UniqueFd(UniqueFd&& other) noexcept : fd(other.fd) {
        other.fd = -1;
    }
    UniqueFd& operator=(UniqueFd&& other) noexcept {
        if (fd >= 0) {
            close(fd);
        }

        fd = other.fd;
        other.fd = -1;
        return *this;
    }

    int get() const noexcept {
       return fd; 
    }

    bool is_valid() {
        return fd >= 0;
    }

private:
    int fd;
};

} // namespace netstack

#endif // NETSTACK_UNIQUE_FD_HPP
