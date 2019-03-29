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

#include <algorithm>
#include <numeric>
#include <iostream>

using namespace seqan3;

template <simd_concept simd_output_t, simd_concept ...simd_ts, typename operation_t>
constexpr simd_output_t simd_transform(operation_t operation, simd_ts const & ...simds)
{
    simd_output_t output{};
    for (size_t i = 0; i < simd_traits<simd_output_t>::length; ++i)
        output[i] = operation(simds[i]...);
    return output;
}

template <simd_concept... simds_t>
constexpr std::tuple<simds_t...> prepare_data()
{
    return {[index = 0](auto simd) mutable
    {
        using simd_t = decltype(simd);
        constexpr auto length = simd_traits<simd_t>::length;
        simd_t iota = simd::iota<simd_t>(0);

        ++index;
        if (index == 1)
            return (iota - 1) * 3; // (i - 1) * 3
        else if (index == 2)
            return length - iota; // length - i
        else if (index == 3)
            return simd_transform<simd_t>([i = 0]() mutable
            {
                return (i++) % 4;
            });
        else
            return iota;
    }(simds_t{})...};
}

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

TEST(simd_algorithm, blend)
{
    using simd_type = simd_type_t<int16_t, 8>;
    using simd_mask_type = typename simd_traits<simd_type>::mask_type;

    auto && [a, b, m] = prepare_data<simd_type, simd_type, simd_mask_type>();

    simd_type result = simd::blend(m, a, b);
    simd_type expect = simd_transform<simd_type>([](auto mi, auto ai, auto bi)
    {
        return mi ? ai : bi;
    }, m, a, b);
    SIMD_EQ(result, expect);
}

TEST(simd_algorithm, min)
{
    using simd_type = simd_type_t<int16_t, 8>;
    auto && [a, b] = prepare_data<simd_type, simd_type>();

    simd_type result = simd::min(a, b);
    simd_type expect = simd_transform<simd_type>([](auto ai, auto bi)
    {
        return std::min(ai, bi);
    }, a, b);
    SIMD_EQ(result, expect);
}

TEST(simd_algorithm, max)
{
    using simd_type = simd_type_t<int16_t, 8>;
    auto && [a, b] = prepare_data<simd_type, simd_type>();

    simd_type result = simd::max(a, b);
    simd_type expect = simd_transform<simd_type>([](auto ai, auto bi)
    {
        return std::max(ai, bi);
    }, a, b);
    SIMD_EQ(result, expect);
}

TEST(simd_algorithm, load)
{
    using simd_type = simd_type_t<int16_t, 8>;
    using scalar_type = typename simd_traits<simd_type>::scalar_type;
    constexpr auto length = simd_traits<simd_type>::length;

    std::array<scalar_type, length> data{};
    std::iota(std::begin(data), std::end(data), 0);

    simd_type result = simd::load<simd_type>(data.data());
    simd_type expect = simd::iota<simd_type>(0);
    SIMD_EQ(result, expect);
}

TEST(simd_algorithm, store_stack)
{
    using simd_type = simd_type_t<int16_t, 8>;
    using scalar_type = typename simd_traits<simd_type>::scalar_type;
    constexpr auto length = simd_traits<simd_type>::length;
    auto && [expect] = prepare_data<simd_type>();

    scalar_type data[length];
    store(data, expect);
    simd_type result = simd::load<simd_type>(data);

    SIMD_EQ(result, expect);
}

TEST(simd_algorithm, store_array)
{
    using simd_type = simd_type_t<int16_t, 8>;
    using scalar_type = typename simd_traits<simd_type>::scalar_type;
    constexpr auto length = simd_traits<simd_type>::length;
    auto && [expect] = prepare_data<simd_type>();

    std::array<scalar_type, length> data;
    store(data.data(), expect);
    simd_type result = simd::load<simd_type>(data.data());

    SIMD_EQ(result, expect);
}

TEST(simd_algorithm, store_heap)
{
    using simd_type = simd_type_t<int16_t, 8>;
    using scalar_type = typename simd_traits<simd_type>::scalar_type;
    constexpr auto length = simd_traits<simd_type>::length;
    auto && [expect] = prepare_data<simd_type>();

    std::vector<scalar_type> data(length);
    store(data.data(), expect);
    simd_type result = simd::load<simd_type>(data.data());

    SIMD_EQ(result, expect);
}
