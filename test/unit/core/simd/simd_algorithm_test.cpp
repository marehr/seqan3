// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE
// -----------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <seqan3/core/simd/simd.hpp>
#include <seqan3/core/simd/simd_algorithm.hpp>
#include <seqan3/test/simd_utility.hpp>

#include <iostream>

using namespace seqan3;

TEST(simd_algorithm, fill)
{
    using simd_type = simd_type_t<int16_t, 8>;

    constexpr simd_type result = simd::fill<simd_type>(4);
    simd_type expect = detail::simd_transform<simd_type>([](size_t)
    {
        return 4;
    });
    SIMD_EQ(result, expect);
}

TEST(simd_algorithm, iota)
{
    using simd_type = simd_type_t<int16_t, 8>;

    constexpr simd_type result = simd::iota<simd_type>(0);
    simd_type expect = detail::simd_transform<simd_type>([i = 0](size_t) mutable
    {
        return i++;
    });
    SIMD_EQ(result, expect);
}

TEST(simd_algorithm, iota_with_offset)
{
    using simd_type = simd_type_t<int16_t, 8>;

    constexpr simd_type result = simd::iota<simd_type>(5);
    simd_type expect = detail::simd_transform<simd_type>([](size_t i)
    {
        return 5 + i;
    });
    SIMD_EQ(result, expect);
}
