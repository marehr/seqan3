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

#include <seqan3/offload/contiguous_container.hpp>
#include <seqan3/test/mpi_gtest.hpp>

using namespace seqan3;
using modes = offload::target_migratable_mode;

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

template <modes mode>
void check_migratable(std::vector<int> expect_vector,
                      seqan3::offload::target_migratable<seqan3::offload::contiguous_container<int>, mode> & migratable)
{
    // std::cout << "check_migratable" << std::endl;
    EXPECT_EQ(migratable.data().size(), 10000u);
    EXPECT_EQ(expect_vector.size(), 10000u);

    // was buffer correctly copied / pushed to the node?
    if constexpr(mode == modes::push_data)
    {
        seqan3::offload::sized_buffer_ptr<int> buffer = migratable.data();
        std::vector<int> result(buffer.get(), buffer.get() + buffer.size());
        EXPECT_EQ(result, expect_vector);
    }

    // did deserialise work?
    {
        // contiguous_container will free the buffer at the end of this scope
        seqan3::offload::contiguous_container<int> container = migratable;
        std::vector<int> result{container.begin(), container.end()};
        EXPECT_EQ(result, expect_vector);
    }

    // NOTE: buffer from migratable will be moved out at this point
    {
        seqan3::offload::sized_buffer_ptr<int> buffer = migratable.data();
        EXPECT_EQ(buffer.get(), nullptr);
        EXPECT_EQ(buffer.size(), 0u);
    }
}

seqan3::offload::target_migratable<seqan3::offload::contiguous_container<int>, modes::pull_data> test_transferred_contiguous_container
(
    seqan3::offload::target_migratable<seqan3::offload::contiguous_container<int>, modes::push_data> migratable
)
{
    std::vector<int> expect(migratable.data().size(), 4);

    check_migratable(std::move(expect), migratable);

    // create a new local buffer and transfer it back to host
    seqan3::offload::contiguous_container<int> container{10000};
    std::fill(container.begin(), container.end(), 8);
    return {std::move(container)};
}

TEST(contiguous_container, target_migratable)
{
    using contiguous_container_t = seqan3::offload::contiguous_container<int>;
    constexpr unsigned size = 10000;

    contiguous_container_t contiguous_container{size};
    std::fill(contiguous_container.begin(), contiguous_container.end(), 4);
    int* begin_ptr = contiguous_container.begin();

    seqan3::offload::node_t node{2};
    seqan3::offload::target_migratable<contiguous_container_t, modes::push_data> arg_migratable{node, std::move(contiguous_container)};

    ham::offload::buffer_ptr<int> buffer = arg_migratable.data();
    EXPECT_EQ(buffer.node(), node);
    EXPECT_NE(buffer.get(), nullptr);
    EXPECT_NE(buffer.get(), begin_ptr);
    EXPECT_EQ(arg_migratable.size(), size);

    seqan3::offload::target_migratable<contiguous_container_t, modes::pull_data> return_migratable = seqan3::test::mpi_gtest<test_transferred_contiguous_container>(node, arg_migratable);

    std::vector<int> expect(size, 8);
    check_migratable(std::move(expect), return_migratable);
}
