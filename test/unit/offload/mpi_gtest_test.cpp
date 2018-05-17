
#include <seqan3/test/mpi_gtest.hpp>

using namespace seqan3::test;

int remote_invoke_test(int size, int parameter)
{
    EXPECT_EQ(size, 3);
    EXPECT_EQ(parameter, 4);
    return parameter;
}

// NOTE: Change a value here, to test if failing will be properly
// propagated
TEST(mpi_gtest, test_invoke)
{
    seqan3::offload::node_t target_node = 1u;
    int result = mpi_gtest<remote_invoke_test>(target_node, 3u, 4u);
    EXPECT_EQ(result, 4);
}

// NOTE: A failing test_invoke should not influence a second invocation
TEST(mpi_gtest, test_invoke2)
{
    seqan3::offload::node_t target_node = 1u;
    mpi_gtest<remote_invoke_test>(target_node, 3u, 4u);
}

void remote_invoke_void_test(int size, int parameter)
{
    EXPECT_EQ(size, 3);
    EXPECT_EQ(parameter, 4);
}

TEST(mpi_gtest, test_invoke_void)
{
    seqan3::offload::node_t target_node = 1u;
    mpi_gtest<remote_invoke_void_test>(target_node, 3u, 4u);
}
