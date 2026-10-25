#include <gtest/gtest.h>
#include <utility>

#include "unique_fd.hpp"
#include "mock_tun_syscalls.hpp"

namespace netstack::test {

TEST(UniqueFdTest, DefaultConstructedDescriptorIsInvalid) {
    UniqueFd fd{};
    EXPECT_FALSE(fd.is_valid());
    EXPECT_EQ(fd.get(), -1);
}

TEST(UniqueFdTest, FdClosedOnDestruction) {
    MockTunSyscalls syscalls{};
    const int _fd = 1;

    EXPECT_CALL(syscalls, close(_fd)).Times(1).WillOnce(::testing::Return(0));
    { UniqueFd fd{_fd, syscalls}; }
}

TEST(UniqueFdTest, GetAndIsValidSuccess) {
    MockTunSyscalls syscalls{};
    const int _fd = 1;
    EXPECT_CALL(syscalls, close(_fd)).WillOnce(::testing::Return(0));
    UniqueFd fd{_fd, syscalls};
    EXPECT_EQ(fd.get(), _fd);
    EXPECT_TRUE(fd.is_valid());
}

TEST(UniqueFdTest, MoveAssignmentTransfersOwnershipAndCloser) {
    ::testing::StrictMock<MockTunSyscalls> old_syscalls;
    ::testing::StrictMock<MockTunSyscalls> new_syscalls;

    const int _fd2 = 2;
    const int _fd1 = 1;
    UniqueFd fd1{_fd1, old_syscalls};
    UniqueFd fd2{_fd2, new_syscalls};

    EXPECT_CALL(old_syscalls, close(_fd1)).Times(1).WillOnce(::testing::Return(0));

    fd1 = std::move(fd2);
    ASSERT_TRUE(::testing::Mock::VerifyAndClearExpectations(&old_syscalls));
    EXPECT_CALL(new_syscalls, close(_fd2)).Times(1).WillOnce(::testing::Return(0));

    EXPECT_EQ(fd1.get(), _fd2);
    EXPECT_TRUE(fd1.is_valid());
    EXPECT_FALSE(fd2.is_valid());
    EXPECT_EQ(fd2.get(), -1);
}

TEST(UniqueFdTest, MoveConstructionTransfersOwnership) {
    ::testing::StrictMock<MockTunSyscalls> syscalls;
    {
        UniqueFd source{123, syscalls};
        {
            UniqueFd destination{std::move(source)};
            EXPECT_EQ(destination.get(), 123);
            EXPECT_TRUE(destination.is_valid());
            EXPECT_EQ(source.get(), -1);
            EXPECT_FALSE(source.is_valid());
            EXPECT_CALL(syscalls, close(123)).Times(1).WillOnce(::testing::Return(0));
        }
        ASSERT_TRUE(::testing::Mock::VerifyAndClearExpectations(&syscalls));
    }
}

TEST(UniqueFdTest, InvalidDescriptorDoesNotClose) {
    ::testing::StrictMock<MockTunSyscalls> syscalls;
    EXPECT_CALL(syscalls, close(::testing::_)).Times(0);
    { UniqueFd fd{-1, syscalls}; }
}

TEST(UniqueFdTest, MoveAssignmentFromInvalidClosesPreviousDescriptor) {
    ::testing::StrictMock<MockTunSyscalls> syscalls;
    UniqueFd source{};
    UniqueFd destination{123, syscalls};
    EXPECT_CALL(syscalls, close(123)).WillOnce(::testing::Return(0));

    destination = std::move(source);

    ASSERT_TRUE(::testing::Mock::VerifyAndClearExpectations(&syscalls));
    EXPECT_EQ(destination.get(), -1);
    EXPECT_FALSE(destination.is_valid());
    EXPECT_FALSE(source.is_valid());
}

TEST(UniqueFdTest, SelfMoveAssignmentPreservesOwnership) {
    ::testing::StrictMock<MockTunSyscalls> syscalls;
    UniqueFd fd{123, syscalls};
    auto &same_fd = fd;
    fd = std::move(same_fd);
    EXPECT_EQ(fd.get(), 123);
    EXPECT_TRUE(fd.is_valid());
    EXPECT_CALL(syscalls, close(123)).Times(1).WillOnce(::testing::Return(0));
}

} // namespace netstack::test
