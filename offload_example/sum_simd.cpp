#include <iostream>
#include <numeric>
#include <thread>

#include <seqan3/io/stream/debug_stream.hpp>
#include <seqan3/std/ranges>

#include "sum_simd.hpp"

int main(int argc, char **argv)
{
    using int32x_t = seqan3::simd::simd_type_t<int>;
    constexpr size_t simd_length = seqan3::simd::simd_traits<int32x_t>::length;

    std::vector<int> numbers{std::ranges::view::iota(0, 1000)};
    seqan3::debug_stream << "vector: " << numbers << std::endl;
    seqan3::debug_stream << "length of simd: " << simd_length << std::endl;

    int sum = sum_simd(numbers);
    std::cout << "result of sum[0,...," << numbers.size() << ") = " << sum << '\n';
    return 0;
}
