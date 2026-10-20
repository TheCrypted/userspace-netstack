#include <gtest/gtest.h>

#include "tun_device.hpp"

namespace {

TEST(TunDeviceTest, RejectsEmptyPacket) {
    // TODO: Add a syscall seam so the test can construct a device without real TUN access.
    // Then verify write_packet rejects an empty span with std::invalid_argument.
    GTEST_SKIP() << "TUN unit tests need a fake syscall implementation";
}

// TODO: Test oversized packets, full writes, EINTR retries, errors, and short writes.

} // namespace
