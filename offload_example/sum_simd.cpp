
#include <iostream>
#include <numeric>
#include <thread>

#include <seqan3/core/simd/simd.hpp>
#include <seqan3/core/simd/simd_algorithm.hpp>
#include <seqan3/io/stream/debug_stream.hpp>
#include <seqan3/std/ranges>

#include "sum_simd.hpp"

int main(int argc, char **argv)
{
    using int32x_t = seqan3::simd::simd_type_t<int>;
    constexpr size_t simd_length = seqan3::simd::simd_traits<int32x_t>::length;

    std::vector<int> numbers{std::ranges::view::iota(0, 100)};
    seqan3::debug_stream << "vector: " << numbers << std::endl;
    seqan3::debug_stream << "length of simd: " << simd_length << std::endl;

    int s = sum_simd(numbers);
    std::cout << "result of sum[0,...," << numbers.size() << ") = " << s << '\n';
    return 0;
}
