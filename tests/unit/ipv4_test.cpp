#include <array>
#include <cstddef>
#include <stdexcept>
#include <gtest/gtest.h>

#include "ipv4.hpp"

namespace netstack::test {

TEST(IPv4Test, ExtractsVersion) {
    // TODO: Pass known packet bytes to netstack::ipv4::version and check the result.
    std::array<std::byte, 4> packet{};
    packet[0] = std::byte{0x45}; // Set first byte to 0100_0101

    EXPECT_EQ(ipv4::version(packet), 4u);

    packet[1] = std::byte{0xff};
    EXPECT_EQ(ipv4::version(packet), 4u);
}

TEST(IPv4Test, ExtractEmptyVersionThrows) {
    EXPECT_THROW(ipv4::version({}), std::invalid_argument);
}

TEST(IPv4Test, Read16ThrowsWrongSize) {
    EXPECT_THROW(ipv4::read_16({}), std::invalid_argument);
}

TEST(IPv4Test, Read16Correct) {
    const std::uint16_t val = 0x1234;
    const std::array bytes{std::byte{0x12}, std::byte{0x34}};

    EXPECT_EQ(ipv4::read_16(bytes), val);
}

TEST(IPv4Test, Read32ThrowsWrongSize) {
    EXPECT_THROW(ipv4::read_32({}), std::invalid_argument);
}

const std::uint32_t val = 0x12345678;
TEST(IPv4Test, Read32Correct) {
    const std::array bytes{
        std::byte{0x12},
        std::byte{0x34},
        std::byte{0x56},
        std::byte{0x78},
    };

    EXPECT_EQ(ipv4::read_32(bytes), val);
}
TEST(IPv4Test, Write16ThrowsWrongSize) {
    EXPECT_THROW(ipv4::write_16({}, 0x1234), std::invalid_argument);
}

TEST(IPv4Test, Write16Correct) {
    std::array<std::byte, 2> bytes{};
    const std::array expected{std::byte{0x12}, std::byte{0x34}};

    ipv4::write_16(bytes, 0x1234);

    EXPECT_EQ(bytes, expected);
}

TEST(IPv4Test, Write32ThrowsWrongSize) {
    EXPECT_THROW(ipv4::write_32({}, 0x12345678), std::invalid_argument);
}

TEST(IPv4Test, Write32Correct) {
    std::array<std::byte, 4> bytes{};
    const std::array expected{std::byte{0x12}, std::byte{0x34}, std::byte{0x56}, std::byte{0x78}};

    ipv4::write_32(bytes, 0x12345678);

    EXPECT_EQ(bytes, expected);
}

} // namespace netstack::test
