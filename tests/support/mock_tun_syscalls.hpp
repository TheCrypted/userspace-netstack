#ifndef NETSTACK_TESTS_MOCK_TUN_SYSCALLS_HPP
#define NETSTACK_TESTS_MOCK_TUN_SYSCALLS_HPP

#include "tun_syscalls.hpp"

#include <gmock/gmock.h>

namespace netstack::test {

class MockTunSyscalls : public TunSyscalls {
public:
    MOCK_METHOD(int, open_device, (), (override));
    MOCK_METHOD(int, attach, (int fd, ifreq &request), (override));
    MOCK_METHOD(ssize_t, read, (int fd, std::span<std::byte> buffer), (override));
    MOCK_METHOD(ssize_t, write, (int fd, std::span<const std::byte> packet), (override));
};

} // namespace netstack::test

#endif // NETSTACK_TESTS_MOCK_TUN_SYSCALLS_HPP
