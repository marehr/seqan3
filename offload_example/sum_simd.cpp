
#include <iostream>
#include <numeric>
#include <thread>

#include <seqan3/core/simd/simd.hpp>
#include <seqan3/io/stream/debug_stream.hpp>
#include <seqan3/std/ranges>

int sum(std::vector<int> numbers)
{
    using int32x_t = seqan3::simd::simd_type_t<int>;
    constexpr size_t simd_length = seqan3::simd::simd_traits<int32x_t>::length;

    seqan3::debug_stream << "vector: " << numbers << std::endl;
    seqan3::debug_stream << "simd_length: " << simd_length << std::endl;

    int const * it = &*std::ranges::begin(numbers);
    int const * it_end = &*std::ranges::end(numbers);
    int const * it_end_peeled = it_end - ((it_end - it) % simd_length);

    int32x_t partial_sums{};

    for (; it < it_end_peeled; it += simd_length)
    {
        // partial_sums += 1;
        partial_sums += simd_load(it);
    }

    int sum = 0;
    for (; it < it_end; ++it)
        sum += *it;
    for (size_t i = 0u; i < simd_length; ++i)
        sum += partial_sums[i];
    return sum;
}

int main(int argc, char **argv)
{
    std::vector<int> numbers{std::ranges::view::iota(0, 100)};
    seqan3::debug_stream << "vector: " << numbers << std::endl;

    int s = sum(numbers);

    std::cout << "result of sum[0,...," << numbers.size() << ") = " << s << '\n';
    return 0;
}
