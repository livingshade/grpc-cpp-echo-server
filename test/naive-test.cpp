#include "gtest/gtest.h"

TEST(Naive, Default)
{
    EXPECT_TRUE(true);
}
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}