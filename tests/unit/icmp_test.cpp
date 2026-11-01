#include "icmp.hpp"

#include <array>
#include <algorithm>
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

TEST(IcmpTest, CreatesEchoHeaderWithPayloadChecksum) {
    auto packet = echo_request;
    // Dirty header ensures all fields, including the old checksum, are replaced.
    std::fill_n(packet.begin(), 8, std::byte{0xff});

    icmp::create_header(packet, 8, 0x1234, 0x5678, packet.size() - 8);

    EXPECT_EQ(packet, echo_request);
}

TEST(IcmpTest, CreateHeaderRejectsShortBuffer) {
    std::array<std::byte, 7> packet{};
    const auto original = packet;

    EXPECT_THROW(icmp::create_header(packet, 8, 0x1234, 0x5678, 0), std::invalid_argument);
    EXPECT_EQ(packet, original);
}

TEST(IcmpTest, CreateReplySuccess) {
    std::array<std::byte, 1> packet{};
    auto req_copy = echo_request;

    int input_payload = 8;
    packet[0] = static_cast<std::byte>(input_payload);
    uint16_t wrong_checksum = 234;
    std::uint16_t input_iden = 0x1234;
    std::uint16_t input_seq = 0x5678;
    icmp::EchoHeader header{
        8, 0, wrong_checksum, input_iden,
        input_seq}; // checksum set to an arbitrary value since it should be ignored
    icmp::create_reply(req_copy, header, packet);

    const auto reply = std::span<const std::byte>{req_copy}.first(8 + packet.size());
    EXPECT_EQ(read_8(reply.subspan(0, 1)), 0u);
    EXPECT_EQ(read_8(reply.subspan(1, 1)), 0u);
    EXPECT_EQ(read_16(reply.subspan(4, 2)), input_iden);
    EXPECT_EQ(read_16(reply.subspan(6, 2)), input_seq);
    EXPECT_EQ(reply[8], packet[0]);
    EXPECT_EQ(internet_checksum(reply), 0u);
}

TEST(IcmpTest, CreateReplyRejectsInsufficientBuffer) {
    const std::array payload{std::byte{0xab}};
    std::array<std::byte, 8> buffer{}; // Nine bytes required for header plus payload.
    buffer.fill(std::byte{0xff});
    const auto original = buffer;
    const icmp::EchoHeader header{8, 0, 0, 0x1234, 0x5678};

    EXPECT_THROW(icmp::create_reply(buffer, header, payload), std::invalid_argument);
    EXPECT_EQ(buffer, original);
}

} // namespace netstack::test
