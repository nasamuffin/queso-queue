#include "../timer.h"

#include "gtest/gtest.h"

TEST(HelloWorld, BasicAssertionWorksWell) {
    EXPECT_EQ(1, 1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
