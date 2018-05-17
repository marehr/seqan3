
#pragma once

#include <gtest/gtest.h>

#include <cassert>
#include <functional>

#include <seqan3/offload/mpi_backend.hpp>

// Use undocumented gtest internals to clear test cases for mpi invocations.
namespace testing::internal
{

class TestResultAccessor {
public:
    static void ClearTestPartResults(TestResult* test_result)
    {
        test_result->ClearTestPartResults();
    }
};

} // testing::internal

namespace seqan3::detail
{

template <auto mpi_test_function, typename ...args_t>
auto mpi_test_function_on_node(args_t ...args)
{
    using namespace ::testing;
    using namespace ::testing::internal;

    using result_type = decltype(mpi_test_function(std::move(args)...));
    constexpr bool is_result_type_void = std::is_same_v<result_type, void>;
    using return_t = std::conditional_t<is_result_type_void, std::tuple<bool>, std::tuple<bool, result_type>>;

    return_t result_tuple;

    if constexpr (!std::is_same_v<result_type, void>)
        result_tuple = return_t{true, mpi_test_function(std::move(args)...)};
    else
        mpi_test_function(std::move(args)...);

    UnitTest & unit_test = *UnitTest::GetInstance();
    const TestResult & ad_hoc_test_result = unit_test.ad_hoc_test_result();
    assert(unit_test.Passed() == ad_hoc_test_result.Passed());

    std::get<0>(result_tuple) = unit_test.Passed();
    TestResultAccessor::ClearTestPartResults(const_cast<TestResult*>(&ad_hoc_test_result));

    return result_tuple;
}

} // namespace seqan3::detail

namespace seqan3::test
{

template <auto mpi_test_function, typename ...args_t>
auto mpi_gtest(seqan3::offload::node_t target_node, args_t ...args)
{
    using namespace seqan3::offload;
    using namespace seqan3::detail;
    constexpr auto function_ptr = mpi_test_function_on_node<mpi_test_function, args_t...>;

    function<function_ptr> test_offload{std::forward<args_t>(args)... };

    std::tuple result_tuple = ham::offload::sync(target_node, test_offload);
    bool mpi_gtest_succeeded = std::get<0>(result_tuple);
    EXPECT_TRUE(mpi_gtest_succeeded) << "Invocation of mpi_gtest returned with failures.";

    // mpi_test_function has a return value, propagate it
    if constexpr(std::tuple_size_v<decltype(result_tuple)> == 2)
        return std::move(std::get<1>(result_tuple));
}

} // namespace seqan3::test
