#include <array>
#include <cstddef>
#include <stdexcept>
#include <gtest/gtest.h>

#include "ipv4.hpp"

namespace {

TEST(IPv4Test, ExtractsVersion) {
    // TODO: Pass known packet bytes to netstack::ipv4::version and check the result.
    std::array<std::byte, 4> packet{};
    packet[0] = std::byte{0x45}; // Set first byte to 0100_0101

    EXPECT_EQ(netstack::ipv4::version(packet), 4u);

    packet[1] = std::byte{0xff};
    EXPECT_EQ(netstack::ipv4::version(packet), 4u);
}

TEST(IPv4Test, ExtractEmptyVersionThrows) {
    EXPECT_THROW(netstack::ipv4::version({}), std::invalid_argument);
}

} // namespace
