#include <array>
#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <gtest/gtest.h>

#include "ipv4.hpp"
#include "packet_utils.hpp"

namespace netstack::test {

TEST(IPv4Test, ExtractsVersion) {
    std::array<std::byte, 4> packet{};
    packet[0] = std::byte{0x45}; // Set first byte to 0100_0101

    EXPECT_EQ(ipv4::version(packet), std::uint8_t{4});

    packet[0] = std::byte{0x4f};
    EXPECT_EQ(ipv4::version(packet), std::uint8_t{4});
}

TEST(IPv4Test, ExtractEmptyVersionThrows) {
    EXPECT_THROW(ipv4::version({}), std::invalid_argument);
}

TEST(IPv4Test, ExtractsIhl) {
    std::array packet{std::byte{0x45}};
    EXPECT_EQ(ipv4::ihl(packet), std::uint8_t{5});

    packet[0] = std::byte{0x65};
    EXPECT_EQ(ipv4::ihl(packet), std::uint8_t{5});
}

TEST(IPv4Test, ExtractEmptyIhlThrows) {
    EXPECT_THROW(ipv4::ihl({}), std::invalid_argument);
}

TEST(IPv4Test, Read8ThrowsWrongSize) {
    EXPECT_THROW(netstack::read_8({}), std::invalid_argument);
}

TEST(IPv4Test, Read8Correct) {
    const std::uint8_t val = 0x12;
    std::array bytes{std::byte{0x12}};

    EXPECT_EQ(netstack::read_8(bytes), val);
}

TEST(IPv4Test, Read16ThrowsWrongSize) {
    EXPECT_THROW(netstack::read_16({}), std::invalid_argument);
}

TEST(IPv4Test, Read16Correct) {
    const std::uint16_t val = 0x1234;
    const std::array bytes{std::byte{0x12}, std::byte{0x34}};

    EXPECT_EQ(netstack::read_16(bytes), val);
}

TEST(IPv4Test, Read32ThrowsWrongSize) {
    EXPECT_THROW(netstack::read_32({}), std::invalid_argument);
}

const std::uint32_t val = 0x12345678;
TEST(IPv4Test, Read32Correct) {
    const std::array bytes{
        std::byte{0x12},
        std::byte{0x34},
        std::byte{0x56},
        std::byte{0x78},
    };

    EXPECT_EQ(netstack::read_32(bytes), val);
}
TEST(IPv4Test, Write16ThrowsWrongSize) {
    EXPECT_THROW(netstack::write_16({}, 0x1234), std::invalid_argument);
}

TEST(IPv4Test, Write16Correct) {
    std::array<std::byte, 2> bytes{};
    const std::array expected{std::byte{0x12}, std::byte{0x34}};

    netstack::write_16(bytes, 0x1234);

    EXPECT_EQ(bytes, expected);
}

TEST(IPv4Test, Write32ThrowsWrongSize) {
    EXPECT_THROW(netstack::write_32({}, 0x12345678), std::invalid_argument);
}

TEST(IPv4Test, Write32Correct) {
    std::array<std::byte, 4> bytes{};
    const std::array expected{std::byte{0x12}, std::byte{0x34}, std::byte{0x56}, std::byte{0x78}};

    netstack::write_32(bytes, 0x12345678);

    EXPECT_EQ(bytes, expected);
}

TEST(IPv4Test, ParsesAllHeaderFields) {
    // 20-byte IPv4 packet, DF set, protocol 253, valid header checksum.
    // Source 192.0.2.1, destination 198.51.100.2.
    const std::array packet{std::byte{0x45}, std::byte{0xb8}, std::byte{0x00}, std::byte{0x14},
                            std::byte{0x12}, std::byte{0x34}, std::byte{0x40}, std::byte{0x00},
                            std::byte{0x40}, std::byte{0xfd}, std::byte{0x3a}, std::byte{0xca},
                            std::byte{0xc0}, std::byte{0x00}, std::byte{0x02}, std::byte{0x01},
                            std::byte{0xc6}, std::byte{0x33}, std::byte{0x64}, std::byte{0x02}};

    const auto header = ipv4::parse_header(packet);

    EXPECT_EQ(header.version, 4u);
    EXPECT_EQ(header.ihl, 5u);
    EXPECT_EQ(header.tos, 0xb8u);
    EXPECT_EQ(header.tot_length, 20u);
    EXPECT_EQ(header.identification, 0x1234u);
    EXPECT_EQ(header.flags, 2u);
    EXPECT_EQ(header.fragment_offset, 0u);
    EXPECT_EQ(header.ttl, 64u);
    EXPECT_EQ(header.protocol, 253u);
    EXPECT_EQ(header.checksum, 0x3acau);
    EXPECT_EQ(header.source_addr, 0xc0000201u);
    EXPECT_EQ(header.dest_addr, 0xc6336402u);
}

TEST(IPv4Test, RejectsIhlBelowMinimum) {
    // IHL is 4 (16 bytes), too short for the mandatory IPv4 fields.
    const std::array packet{std::byte{0x44}, std::byte{0xb8}, std::byte{0x00}, std::byte{0x14},
                            std::byte{0x12}, std::byte{0x34}, std::byte{0x40}, std::byte{0x00},
                            std::byte{0x40}, std::byte{0xfd}, std::byte{0x3b}, std::byte{0xca},
                            std::byte{0xc0}, std::byte{0x00}, std::byte{0x02}, std::byte{0x01},
                            std::byte{0xc6}, std::byte{0x33}, std::byte{0x64}, std::byte{0x02}};

    EXPECT_THROW(ipv4::parse_header(packet), std::invalid_argument);
}

TEST(IPv4Test, RejectsHeaderLengthBeyondBuffer) {
    std::array<std::byte, 20> packet{};
    packet[0] = std::byte{0x46}; // IHL 6 declares a 24-byte header.
    packet[3] = std::byte{0x18}; // Total length 24.

    EXPECT_THROW(ipv4::parse_header(packet), std::invalid_argument);
}

TEST(IPv4Test, RejectsTotalLengthBelowHeaderLength) {
    std::array<std::byte, 20> packet{};
    packet[0] = std::byte{0x45};
    packet[3] = std::byte{0x13}; // Total length 19, header length 20.

    EXPECT_THROW(ipv4::parse_header(packet), std::invalid_argument);
}

TEST(IPv4Test, RejectsTotalLengthBeyondBuffer) {
    std::array<std::byte, 20> packet{};
    packet[0] = std::byte{0x45};
    packet[3] = std::byte{0x15}; // Total length 21, received size 20.

    EXPECT_THROW(ipv4::parse_header(packet), std::invalid_argument);
}

namespace {
constexpr std::array checksum_packet{
    std::byte{0x45}, std::byte{0xb8}, std::byte{0x00}, std::byte{0x14}, std::byte{0x12},
    std::byte{0x34}, std::byte{0x40}, std::byte{0x00}, std::byte{0x40}, std::byte{0xfd},
    std::byte{0x3a}, std::byte{0xca}, std::byte{0xc0}, std::byte{0x00}, std::byte{0x02},
    std::byte{0x01}, std::byte{0xc6}, std::byte{0x33}, std::byte{0x64}, std::byte{0x02}};
}

TEST(IPv4Test, ChecksumAcceptsKnownHeader) {
    EXPECT_NO_THROW(ipv4::parse_ip_packet(checksum_packet));
}

TEST(IPv4Test, ChecksumRejectsCorruption) {
    auto packet = checksum_packet;
    packet[11] = std::byte{0xcb}; // Corrupt the stored checksum.
    EXPECT_THROW(ipv4::parse_ip_packet(packet), std::invalid_argument);

    packet = checksum_packet;
    packet[8] = std::byte{0x41}; // Change TTL without updating checksum.
    EXPECT_THROW(ipv4::parse_ip_packet(packet), std::invalid_argument);
}

TEST(IPv4Test, ChecksumExcludesPayload) {
    std::array<std::byte, 21> packet{};
    std::copy(checksum_packet.begin(), checksum_packet.end(), packet.begin());
    packet[3] = std::byte{0x15}; // Total length 21; adjusted header checksum 0x3ac9.
    packet[11] = std::byte{0xc9};
    packet[20] = std::byte{0xff};

    EXPECT_NO_THROW(ipv4::parse_ip_packet(packet));
    packet[20] = std::byte{0x12};
    EXPECT_NO_THROW(ipv4::parse_ip_packet(packet));
}

TEST(IPv4Test, ChecksumFoldsRepeatedCarries) {
    // Synthetic words: 0xffff + 0xffff + 1 folds to 1; complement is 0xfffe.
    std::array<std::byte, 20> packet{};
    packet[0] = packet[1] = packet[2] = packet[3] = std::byte{0xff};
    packet[5] = std::byte{0x01};
    packet[10] = std::byte{0xff};
    packet[11] = std::byte{0xfe};

    EXPECT_EQ(netstack::internet_checksum(packet), 0u);
}

TEST(IPv4Test, ParseIPV4ParsesCorrectHeaderAndData) {
    std::array<std::byte, 21> packet{};
    std::copy(checksum_packet.begin(), checksum_packet.end(), packet.begin());
    packet[3] = std::byte{0x15}; // Total length 21; adjusted header checksum 0x3ac9.
    packet[11] = std::byte{0xc9};
    packet[20] = std::byte{0xff};

    auto ip_packet = ipv4::parse_ip_packet(packet);
    EXPECT_EQ(ip_packet.header.tot_length, 21u);
    EXPECT_EQ(ip_packet.header.source_addr, 0xc0000201u);
    ASSERT_EQ(ip_packet.data.size(), 1u);
    EXPECT_EQ(ip_packet.data[0], packet[20]);
}
} // namespace netstack::test
