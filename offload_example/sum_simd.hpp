#pragma once

#include <seqan3/core/simd/simd.hpp>
#include <seqan3/core/simd/simd_algorithm.hpp>

#include "sum_serial.hpp"

int sum_simd(std::span<int const> numbers)
{
    using int32x_t = seqan3::simd::simd_type_t<int>;
    constexpr size_t simd_length = seqan3::simd::simd_traits<int32x_t>::length;

    int const * it = &*std::ranges::begin(numbers);
    int const * it_end = &*std::ranges::end(numbers);
    int const * it_end_peeled = it_end - ((it_end - it) % simd_length);

    int sum = 0;
    int32x_t partial_sums{};

    // simd kernel
    for (; it < it_end_peeled; it += simd_length)
        partial_sums += seqan3::simd::load<int32x_t>(it);

    // add remainder loop
    sum += sum_serial(std::span{it, it_end});

    // construct complete sum
    for (size_t i = 0u; i < simd_length; ++i)
        sum += partial_sums[i];
    return sum;
}
