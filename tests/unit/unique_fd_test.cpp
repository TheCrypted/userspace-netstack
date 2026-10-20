#include <gtest/gtest.h>

#include "unique_fd.hpp"

namespace {

TEST(UniqueFdTest, DefaultConstructedDescriptorIsInvalid) {
    // TODO: Construct netstack::UniqueFd and check get() and is_valid().
    GTEST_SKIP() << "Default descriptor test not implemented yet";
}

// TODO: Test closing on destruction and ownership transfer through moves.

} // namespace
