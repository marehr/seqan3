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

#include <seqan3/offload/sized_buffer_ptr.hpp>

TEST(sized_buffer_ptr, constructor)
{
    EXPECT_TRUE(std::is_default_constructible_v<seqan3::offload::sized_buffer_ptr<int>>);
    EXPECT_TRUE(std::is_copy_constructible_v<seqan3::offload::sized_buffer_ptr<int>>);
    EXPECT_TRUE(std::is_move_constructible_v<seqan3::offload::sized_buffer_ptr<int>>);
}

TEST(sized_buffer_ptr, deconstructor)
{
    EXPECT_TRUE(std::is_destructible_v<seqan3::offload::sized_buffer_ptr<int>>);
}

TEST(sized_buffer_ptr, assignment)
{
    EXPECT_TRUE(std::is_copy_assignable_v<seqan3::offload::sized_buffer_ptr<int>>);
    EXPECT_TRUE(std::is_move_assignable_v<seqan3::offload::sized_buffer_ptr<int>>);
}

TEST(sized_buffer_ptr, move_constructor_with_swap_semantics)
{
    std::unique_ptr<int[]> buffer_ptr(new int[300u]);

    seqan3::offload::sized_buffer_ptr<int> buffer{{buffer_ptr.get(), seqan3::offload::node_t{1}}, 300u};
    size_t size = buffer.size();

    EXPECT_NE(buffer_ptr.get(), nullptr);
    EXPECT_EQ(size, 300u);

    seqan3::offload::sized_buffer_ptr<int> buffer2{std::move(buffer)};

    EXPECT_EQ(buffer.get(), nullptr);
    EXPECT_EQ(buffer.size(), 0u);

    EXPECT_EQ(buffer2.get(), buffer_ptr.get());
    EXPECT_EQ(buffer2.size(), size);
}

TEST(sized_buffer_ptr, move_assignment_with_swap_semantics)
{
    std::unique_ptr<int[]> buffer_ptr(new int[300u]);
    std::unique_ptr<int[]> buffer_ptr2(new int[200u]);

    seqan3::offload::sized_buffer_ptr<int> buffer{{buffer_ptr.get(), seqan3::offload::node_t{1}}, 300u};
    seqan3::offload::sized_buffer_ptr<int> buffer2{{buffer_ptr2.get(), seqan3::offload::node_t{1}}, 200u};
    size_t size = buffer.size();

    buffer2 = std::move(buffer);

    EXPECT_EQ(buffer.get(), nullptr);
    EXPECT_EQ(buffer.size(), 0u);

    EXPECT_EQ(buffer2.get(), buffer_ptr.get());
    EXPECT_EQ(buffer2.size(), size);
}

bool transferable_test
(
    seqan3::offload::sized_buffer_ptr<int> buffer_ptr,
    size_t size,
    int* pointer
)
{
    EXPECT_EQ(buffer_ptr.get(), pointer);
    EXPECT_EQ(buffer_ptr.size(), size);
    EXPECT_EQ(buffer_ptr.node(), 0);
    return true;
}

TEST(sized_buffer_ptr, transferable)
{
    using sized_buffer_ptr_t = seqan3::offload::sized_buffer_ptr<int>;
    constexpr unsigned size = 10000;
    seqan3::offload::node_t host_node = ham::offload::this_node();
    seqan3::offload::node_t target_node = 2;

    std::unique_ptr<int[]> buffer_ptr(new int[size]);
    std::fill(buffer_ptr.get(), buffer_ptr.get() + size, 4);

    sized_buffer_ptr_t sized_buffer_ptr{{buffer_ptr.get(), host_node}, size};

    seqan3::offload::function<transferable_test> offload_test{
        sized_buffer_ptr,
        size,
        buffer_ptr.get()
    };
    bool succeeded = ham::offload::sync(target_node, offload_test);
    EXPECT_TRUE(succeeded);
}
