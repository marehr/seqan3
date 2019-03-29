#include <iostream>
#include <numeric>
#include <thread>

#include <seqan3/io/stream/debug_stream.hpp>
#include <seqan3/std/ranges>

#include "sum_serial.hpp"

int main(int argc, char **argv)
{
    std::vector<int> numbers{std::ranges::view::iota(0, 1000)};
    seqan3::debug_stream << "vector: " << numbers << std::endl;

    int sum = sum_serial(numbers);
    std::cout << "result of sum[0,...," << numbers.size() << ") = " << sum << '\n';
    return 0;
}
