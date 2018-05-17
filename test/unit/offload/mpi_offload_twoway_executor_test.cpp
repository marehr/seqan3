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

#include <seqan3/offload/mpi_offload.hpp>

#include <seqan3/offload/contiguous_container.hpp>

using namespace seqan3;

int forty_two_fn()
{
    return 45;
}

int add_fn(int x, int y)
{
    return x + y;
}

TEST(twoway_executor, is_twoway_executor)
{
    mpi_offload mpi_offloader{1};
    auto mpi_ex = mpi_offloader.executor();

    EXPECT_TRUE((seqan3::execution::is_twoway_executor_v<decltype(mpi_ex)>));
}

TEST(twoway_executor, functions_with_no_offload)
{
    mpi_offload mpi_offloader{1};
    auto mpi_ex = mpi_offloader.executor();

    auto lambda = []
    {
        return 43;
    };

    // will NOT be offloaded
    auto f1 = mpi_ex.twoway_execute(lambda);
    auto f2 = mpi_ex.twoway_execute([=]
    {
        return lambda() + 2;
    });

    EXPECT_EQ(f1.get(), 43);
    EXPECT_EQ(f2.get(), 45);
}

TEST(twoway_executor, offload_functions_with_basic_types)
{
    mpi_offload mpi_offloader{1};
    auto mpi_ex = mpi_offloader.executor();

    int x = 15;
    int y = 20;

    // will be offloaded
    auto f1 = mpi_ex.twoway_execute(seqan3::offload::function<forty_two_fn>{});
    auto f2 = mpi_ex.twoway_execute(seqan3::offload::function<add_fn>{x, y});
    auto f3 = mpi_ex.twoway_execute(seqan3::offload::function<add_fn>{15, 20});

    EXPECT_EQ(f1.get(), 45);
    EXPECT_EQ(f2.get(), 15 + 20);
    EXPECT_EQ(f3.get(), 15 + 20);
}
