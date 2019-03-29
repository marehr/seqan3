#include <iostream>
#include <numeric>
#include <thread>

#include <seqan3/io/stream/debug_stream.hpp>

#include "sum_simd.hpp"
#include "sum_thread.hpp"

int main(int argc, char **argv)
{
    std::vector<int> numbers{std::ranges::view::iota(0, 1000)};
    seqan3::debug_stream << "vector: " << numbers << std::endl;

    size_t threads = std::thread::hardware_concurrency();
    int sum = sum_thread<sum_simd>(numbers, threads);
    std::cout << "result of sum[0,...," << numbers.size() << ") = " << sum << '\n';
    return 0;
}
