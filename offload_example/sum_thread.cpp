
#include <iostream>
#include <numeric>
#include <thread>

#include <seqan3/io/stream/debug_stream.hpp>
#include <seqan3/std/ranges>
#include <experimental/thread_pool>

#include "sum_serial.hpp"

int sum_thread(std::vector<int> numbers)
{
    size_t threads = std::thread::hardware_concurrency();
    seqan3::debug_stream << "number of threads: " << threads << std::endl;

    // std::experimental::static_thread_pool pool{threads};
    // pool.bulk_execute([](int i, std::ignore_t)
    // {
    //
    // });
    return 0;
}

int main(int argc, char **argv)
{
    std::vector<int> numbers{std::ranges::view::iota(0, 100)};
    seqan3::debug_stream << "vector: " << numbers << std::endl;

    int s = sum_thread(numbers);
    std::cout << "result of sum[0,...," << numbers.size() << ") = " << s << '\n';
    return 0;
}
