#include <iostream>
#include <numeric>
#include <thread>

#include <seqan3/io/stream/debug_stream.hpp>

#include "sum_offload.hpp"
#include "sum_thread.hpp"

int main(int argc, char **argv)
{
    size_t nodes = seqan3::offload::num_nodes();
    std::cout << "number of nodes: " << nodes << std::endl;

    std::vector<int> numbers{std::ranges::view::iota(0, 1000)};
    seqan3::debug_stream << "vector: " << numbers << std::endl;

    int sum = sum_offload_host(numbers);
    std::cout << "result of sum[0,...," << numbers.size() << ") = " << sum << '\n';
    return 0;
}

int sum_offload_node(std::vector<int> numbers)
{
    size_t nodes = seqan3::offload::num_nodes();
    size_t threads = std::thread::hardware_concurrency() / nodes;
    seqan3::debug_stream << "number of threads: " << threads << std::endl;
    seqan3::debug_stream << "called on node " << seqan3::offload::this_node() << '\n';
    seqan3::debug_stream << "vector: " << trim(numbers) << std::endl;
    return sum_thread<sum_serial>(numbers, threads);
}
