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

#include <seqan3/test/mpi_gtest.hpp>

#include <seqan3/offload/contiguous_container.hpp>

TEST(contiguous_container, constructor)
{
    EXPECT_TRUE(std::is_default_constructible_v<seqan3::offload::contiguous_container<int>>);
    EXPECT_FALSE(std::is_copy_constructible_v<seqan3::offload::contiguous_container<int>>);
    EXPECT_TRUE(std::is_move_constructible_v<seqan3::offload::contiguous_container<int>>);
}

TEST(contiguous_container, deconstructor)
{
    EXPECT_TRUE(std::is_destructible_v<seqan3::offload::contiguous_container<int>>);
}

TEST(contiguous_container, assignment)
{
    EXPECT_FALSE(std::is_copy_assignable_v<seqan3::offload::contiguous_container<int>>);
    EXPECT_TRUE(std::is_move_assignable_v<seqan3::offload::contiguous_container<int>>);
}

TEST(contiguous_container, move_constructor_with_swap_semantics)
{
    seqan3::offload::contiguous_container<int> buffer{300};
    int * buffer_ptr = buffer.begin();
    size_t size = buffer.size();

    seqan3::offload::contiguous_container<int> buffer2{std::move(buffer)};

    EXPECT_EQ(buffer.begin(), nullptr);
    EXPECT_EQ(buffer.size(), 0u);

    EXPECT_EQ(buffer2.begin(), buffer_ptr);
    EXPECT_EQ(buffer2.size(), size);
}

TEST(contiguous_container, move_assignment_with_swap_semantics)
{
    seqan3::offload::contiguous_container<int> buffer{300};
    seqan3::offload::contiguous_container<int> buffer2{200};
    int * buffer_ptr = buffer.begin();
    size_t size = buffer.size();

    buffer2 = std::move(buffer);

    EXPECT_EQ(buffer.begin(), nullptr);
    EXPECT_EQ(buffer.size(), 0u);

    EXPECT_EQ(buffer2.begin(), buffer_ptr);
    EXPECT_EQ(buffer2.size(), size);
}

TEST(contiguous_container, is_contiguous_container)
{
    using namespace seqan3::offload;

    EXPECT_FALSE((is_contiguous_container<int>::value));
    EXPECT_TRUE((is_contiguous_container<contiguous_container<int>>::value));
    EXPECT_TRUE((is_contiguous_container<const contiguous_container<int>>::value));
    EXPECT_TRUE((is_contiguous_container<const contiguous_container<int> &>::value));
    EXPECT_TRUE((is_contiguous_container<contiguous_container<int> &&>::value));
    EXPECT_TRUE((is_contiguous_container<volatile contiguous_container<int> &&>::value));
}

void test_transferred_contiguous_container
(
    seqan3::offload::target_migratable<seqan3::offload::contiguous_container<int>> migratable,
    size_t size
)
{
    std::vector<int> expect(size, 4);

    // was buffer correctly copied?
    {
        ham::offload::buffer_ptr<int> buffer = migratable.data();
        std::vector<int> result{buffer.get(), buffer.get() + size};
        EXPECT_EQ(result, expect);
    }

    // did deserialise work?
    {
        seqan3::offload::contiguous_container<int> buffer = migratable;
        std::vector<int> result{buffer.begin(), buffer.end()};
        EXPECT_EQ(result, expect);
    }
}

TEST(contiguous_container, target_migratable)
{
    using contiguous_container_t = seqan3::offload::contiguous_container<int>;
    constexpr unsigned size = 10000;

    contiguous_container_t contiguous_container{size};
    std::fill(contiguous_container.begin(), contiguous_container.end(), 4);
    int* begin_ptr = contiguous_container.begin();

    seqan3::offload::node_t node{2};
    seqan3::offload::target_migratable<contiguous_container_t> migratable{node, std::move(contiguous_container)};

    ham::offload::buffer_ptr<int> buffer = migratable.data();
    EXPECT_EQ(buffer.node(), node);
    EXPECT_NE(buffer.get(), nullptr);
    EXPECT_NE(buffer.get(), begin_ptr);
    EXPECT_EQ(migratable.size(), size);

    seqan3::test::mpi_gtest<test_transferred_contiguous_container>(node, migratable, size);
}
