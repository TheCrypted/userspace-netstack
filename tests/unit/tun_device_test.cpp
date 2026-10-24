#include <gtest/gtest.h>

#include "tun_device.hpp"
#include "mock_tun_syscalls.hpp"

#include <memory>
#include <array>
#include <algorithm>
#include <cerrno>
#include <system_error>

namespace netstack::test {

class TunDeviceTest : public ::testing::Test {
protected:
    MockTunSyscalls syscalls;
    std::string name{"tun0"};
    std::unique_ptr<TunDevice> td;
    const int fd{1};

    void create_device() {
        EXPECT_CALL(syscalls, open_device()).WillOnce(::testing::Return(fd));
        EXPECT_CALL(syscalls, attach(fd, ::testing::_)).WillOnce(::testing::Return(0));

        td = std::make_unique<TunDevice>(name, syscalls);

        EXPECT_CALL(syscalls, close(fd)).WillOnce(::testing::Return(0));
    }

    void SetUp() override { create_device(); }
};

TEST_F(TunDeviceTest, WriteRejectsEmptyPacket) {
    EXPECT_CALL(syscalls, write(::testing::_, ::testing::_)).Times(0);
    EXPECT_THROW(td->write_packet(std::span<const std::byte>{}), std::invalid_argument);
};

TEST_F(TunDeviceTest, WriteRejectsLargePacket) {
    EXPECT_CALL(syscalls, write(::testing::_, ::testing::_)).Times(0);
    EXPECT_THROW(td->write_packet(std::array<const std::byte, max_ipv4_packet_size + 1>{}),
                 std::invalid_argument);
};

TEST_F(TunDeviceTest, WriteRetriesOnEINTR) {
    const std::array packet{std::byte{0x45}};

    EXPECT_CALL(syscalls, write(fd, ::testing::_))
        .WillOnce(::testing::Invoke([](int, std::span<const std::byte>) -> ssize_t {
            errno = EINTR;
            return -1;
        }))
        .WillOnce(::testing::Return(static_cast<ssize_t>(packet.size())));

    EXPECT_EQ(td->write_packet(packet), packet.size());
};

TEST_F(TunDeviceTest, WriteSuccess) {
    const std::array packet{std::byte{0x45}};

    EXPECT_CALL(syscalls, write(fd, ::testing::_))
        .WillOnce(::testing::Return(static_cast<ssize_t>(packet.size())));

    EXPECT_EQ(td->write_packet(packet), packet.size());
};

TEST_F(TunDeviceTest, WriteThrowsOnIncorrectReturnSize) {
    const std::array packet{std::byte{0x45}};
    ssize_t wrong_return = packet.size() - 1;

    EXPECT_CALL(syscalls, write(fd, ::testing::_))
        .WillOnce(::testing::Return(static_cast<ssize_t>(wrong_return)));

    EXPECT_THROW(td->write_packet(packet), std::runtime_error);
};

TEST_F(TunDeviceTest, WriteThrowsOnSyscallFailure) {
    const std::array packet{std::byte{0x45}};
    EXPECT_CALL(syscalls, write(fd, ::testing::_))
        .WillOnce(::testing::Invoke([](int, std::span<const std::byte>) -> ssize_t {
            errno = EIO;
            return -1;
        }));

    try {
        td->write_packet(packet);
        FAIL() << "Expected std::system_error";
    } catch (const std::system_error &error) {
        EXPECT_EQ(error.code(), std::error_code(EIO, std::generic_category()));
    }
}

TEST_F(TunDeviceTest, ReadReturnsPacketInBuffer) {
    const std::array packet{std::byte{0x45}, std::byte{0x00}, std::byte{0xff}};
    EXPECT_CALL(syscalls, read(fd, ::testing::_))
        .WillOnce(::testing::Invoke([&](int, std::span<std::byte> buffer) -> ssize_t {
            EXPECT_EQ(buffer.data(), td->buffer.data());
            EXPECT_EQ(buffer.size(), td->buffer.size());
            std::copy(packet.begin(), packet.end(), buffer.begin());
            return static_cast<ssize_t>(packet.size());
        }));

    EXPECT_EQ(td->read_packet(), packet.size());
    EXPECT_TRUE(std::equal(packet.begin(), packet.end(), td->buffer.begin()));
}

TEST_F(TunDeviceTest, ReadRetriesOnEINTR) {
    EXPECT_CALL(syscalls, read(fd, ::testing::_))
        .WillOnce(::testing::Invoke([](int, std::span<std::byte>) -> ssize_t {
            errno = EINTR;
            return -1;
        }))
        .WillOnce(::testing::Invoke([](int, std::span<std::byte> buffer) -> ssize_t {
            buffer[0] = std::byte{0x45};
            return 1;
        }));

    EXPECT_EQ(td->read_packet(), 1u);
    EXPECT_EQ(td->buffer[0], std::byte{0x45});
}

TEST_F(TunDeviceTest, ReadThrowsOnSyscallFailure) {
    EXPECT_CALL(syscalls, read(fd, ::testing::_))
        .WillOnce(::testing::Invoke([](int, std::span<std::byte>) -> ssize_t {
            errno = EIO;
            return -1;
        }));

    try {
        td->read_packet();
        FAIL() << "Expected std::system_error";
    } catch (const std::system_error &error) {
        EXPECT_EQ(error.code(), std::error_code(EIO, std::generic_category()));
    }
}

} // namespace netstack::test
