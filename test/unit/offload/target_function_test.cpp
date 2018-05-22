// ==========================================================================
//                 SeqAn - The Library for Sequence Analysis
// ==========================================================================
//
// Copyright (c) 2006-2017, Knut Reinert, FU Berlin
// Copyright (c) 2016-2017, Knut Reinert & MPI Molekulare Genetik
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Knut Reinert or the FU Berlin nor the names of
//       its contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL KNUT REINERT OR THE FU BERLIN BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// ==========================================================================

#include <gtest/gtest.h>

#include <experimental/thread_pool>

#include <utility>
#include <string>
#include <numeric>

#include <seqan3/offload/target_function.hpp>

#include <seqan3/offload/contiguous_container.hpp>

using namespace seqan3;

int forty_two_fn()
{
    return 45;
}

TEST(forty_two_fn, arguments_type)
{
    using target_function_t = seqan3::offload::target_function<forty_two_fn>;

    using expected_arguments_type = std::tuple<>;
    using actual_arguments_type = typename target_function_t::arguments_type;
    EXPECT_TRUE((std::is_same_v<expected_arguments_type, actual_arguments_type>));
}

TEST(forty_two_fn, result_type)
{
    using target_function_t = seqan3::offload::target_function<forty_two_fn>;

    using expected_result_type = int;
    using actual_result_type = typename target_function_t::result_type;
    EXPECT_TRUE((std::is_same_v<expected_result_type, actual_result_type>));
}

TEST(forty_two_fn, remote_execution)
{
    using target_function_t = seqan3::offload::target_function<forty_two_fn>;

    offload::node_t target = 2;

    auto f1 = target_function_t{target}.async();

    EXPECT_EQ(f1.get(), 45);
}

int add_fn(int x, int y)
{
    return x + y;
}

TEST(add_fn, arguments_type)
{
    using target_function_t = seqan3::offload::target_function<add_fn>;

    using expected_arguments_type = std::tuple<
        offload::target_migratable<int>,
        offload::target_migratable<int>
    >;
    using actual_arguments_type = typename target_function_t::arguments_type;
    EXPECT_TRUE((std::is_same_v<expected_arguments_type, actual_arguments_type>));
}

TEST(add_fn, result_type)
{
    using target_function_t = seqan3::offload::target_function<add_fn>;

    using expected_result_type = int;
    using actual_result_type = typename target_function_t::result_type;
    EXPECT_TRUE((std::is_same_v<expected_result_type, actual_result_type>));
}

TEST(add_fn, remote_execution)
{
    using target_function_t = seqan3::offload::target_function<add_fn>;

    offload::node_t target = 2;

    int x = 15;
    int y = 21;
    auto f1 = target_function_t{target, x, y}.async();
    auto f2 = target_function_t{target, 15, 20}.async();

    EXPECT_EQ(f1.get(), 15 + 21);
    EXPECT_EQ(f2.get(), 15 + 20);
}

int accumulate_buffer_ptr(ham::offload::buffer_ptr<int> numbers_buffer, size_t n)
{
    int * numbers = numbers_buffer.get();
    return std::accumulate(numbers, numbers+n, 0);
}

TEST(accumulate_buffer_ptr, arguments_type)
{
    using target_function_t = seqan3::offload::target_function<accumulate_buffer_ptr>;

    using expected_arguments_type = std::tuple<
        offload::target_migratable<ham::offload::buffer_ptr<int>>,
        offload::target_migratable<size_t>
    >;
    using actual_arguments_type = typename target_function_t::arguments_type;
    EXPECT_TRUE((std::is_same_v<expected_arguments_type, actual_arguments_type>));
}

TEST(accumulate_buffer_ptr, result_type)
{
    using target_function_t = seqan3::offload::target_function<accumulate_buffer_ptr>;

    using expected_result_type = int;
    using actual_result_type = typename target_function_t::result_type;
    EXPECT_TRUE((std::is_same_v<expected_result_type, actual_result_type>));
}

TEST(accumulate_buffer_ptr, remote_execution)
{
    using target_function_t = seqan3::offload::target_function<accumulate_buffer_ptr>;

    offload::node_t target = 2;

    int numbers[]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    constexpr size_t n = sizeof(numbers) / sizeof(int);
    EXPECT_EQ(n, 11u);

    ham::offload::buffer_ptr<int> numbers_buffer = ham::offload::allocate<int>(target, n);
    ham::offload::put(numbers, numbers_buffer, n);

    auto f1 = target_function_t{target, numbers_buffer, n}.async();
    EXPECT_EQ(f1.get(), 11 * 5);
}

template <typename range_t>
auto accumulate(range_t numbers)
{
    return std::accumulate(numbers.begin(), numbers.end(), 0);
}

template <typename tuple>
class offload_accumulate : public ::testing::Test
{
public:
    static constexpr int size = std::tuple_element_t<0, tuple>{};
    using range_t = std::tuple_element_t<1, tuple>;
    using target_function_t = seqan3::offload::target_function<accumulate<range_t>>;
};

using range_types = ::testing::Types <
    // TODO: The array in this test case is to big and gets a MPI exception
    // std::tuple<std::integral_constant<int, 1006>, std::array<int, 1006>>, // 1005 works on my system
    std::tuple<std::integral_constant<int, 10>, std::array<int, 10>>,
    std::tuple<std::integral_constant<int, 10>, std::array<int, 10> const &>,
    std::tuple<std::integral_constant<int, 10>, std::array<int, 10> &&>,
    std::tuple<std::integral_constant<int, 10000000>, seqan3::offload::contiguous_container<int>>,
    std::tuple<std::integral_constant<int, 10000000>, seqan3::offload::contiguous_container<int> const &>,
    std::tuple<std::integral_constant<int, 10000000>, seqan3::offload::contiguous_container<int> &&>
>;

TYPED_TEST_CASE(offload_accumulate, range_types);

TYPED_TEST(offload_accumulate, arguments_type)
{
    using range_t = typename TestFixture::range_t;
    using target_function_t = typename TestFixture::target_function_t;

    using expected_type = std::tuple<
        offload::target_migratable<range_t>
    >;
    using actual_type = typename target_function_t::arguments_type;
    EXPECT_TRUE((std::is_same_v<expected_type, actual_type>));
}

TYPED_TEST(offload_accumulate, result_type)
{
    using target_function_t = typename TestFixture::target_function_t;

    using expected_type = int;
    using actual_type = typename target_function_t::result_type;
    EXPECT_TRUE((std::is_same_v<expected_type, actual_type>));
}

TYPED_TEST(offload_accumulate, remote_execution)
{
    constexpr int size = TestFixture::size;
    using range_t = typename TestFixture::range_t;
    using target_function_t = typename TestFixture::target_function_t;

    offload::node_t target = 2;

    std::decay_t<range_t> numbers{size};
    std::for_each(numbers.begin(), numbers.end(), [](int &i){ i = 2; });

    auto f1 = target_function_t{target, std::move(numbers)}.async();
    EXPECT_EQ(f1.get(), 2 * size);
}
