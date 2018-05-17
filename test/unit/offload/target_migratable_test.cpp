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

#include <type_traits>

#include <seqan3/offload/target_migratable.hpp>

using namespace seqan3;

template <typename value_t>
struct default_target_migratable : offload::target_migratable<value_t>
{
    using offload::target_migratable<value_t>::value;
};

template <typename value_t>
auto migratable_value(const offload::target_migratable<value_t> & migratable)
{
    return reinterpret_cast<const default_target_migratable<value_t>*>(&migratable)->value;
}

TEST(target_migratable, constructor)
{
    EXPECT_FALSE(std::is_default_constructible_v<offload::target_migratable<int>>);
    EXPECT_TRUE(std::is_copy_constructible_v<offload::target_migratable<int>>);
    EXPECT_TRUE(std::is_move_constructible_v<offload::target_migratable<int>>);
}

TEST(target_migratable, deconstructor)
{
    EXPECT_TRUE(std::is_destructible_v<offload::target_migratable<int>>);
}

TEST(target_migratable, assignment)
{
    EXPECT_TRUE(std::is_copy_assignable_v<offload::target_migratable<int>>);
    EXPECT_TRUE(std::is_move_assignable_v<offload::target_migratable<int>>);
}

TEST(default_target_migratable, serialise_and_deserialise)
{
    offload::node_t node{1};
    offload::target_migratable<int> migratable{node, 5}; // serialise
    EXPECT_EQ(migratable_value(migratable), 5); // internal value
    EXPECT_EQ(static_cast<int>(migratable), 5); // deserialise
}
