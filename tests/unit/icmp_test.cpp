#include "icmp.hpp"

#include <array>
#include <cstddef>
#include <stdexcept>
#include <gtest/gtest.h>

namespace netstack::test {
namespace {
// Echo request with identifier 0x1234, sequence 0x5678, and odd-length payload.
constexpr std::array echo_request{std::byte{0x08}, std::byte{0x00}, std::byte{0xf2},
                                  std::byte{0xa4}, std::byte{0x12}, std::byte{0x34},
                                  std::byte{0x56}, std::byte{0x78}, std::byte{0xde},
                                  std::byte{0xad}, std::byte{0xbe}};
} // namespace

TEST(IcmpTest, ParsesEchoHeader) {
    const auto header = icmp::parse_header(echo_request);

    EXPECT_EQ(header.type, 8u);
    EXPECT_EQ(header.code, 0u);
    EXPECT_EQ(header.checksum, 0xf2a4u);
    EXPECT_EQ(header.identifier, 0x1234u);
    EXPECT_EQ(header.seq_num, 0x5678u);
}

TEST(IcmpTest, RejectsTruncatedEchoHeader) {
    EXPECT_THROW(icmp::parse_header(std::span{echo_request}.first(7)), std::invalid_argument);
}

TEST(IcmpTest, RejectsInvalidChecksum) {
    auto packet = echo_request;
    packet.back() = std::byte{0xbf}; // Corrupt payload without updating the checksum.

    EXPECT_THROW(icmp::parse_header(packet), std::invalid_argument);
}

} // namespace netstack::test
