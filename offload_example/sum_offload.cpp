
#include <iostream>
#include <numeric>
#include <thread>

#include <seqan3/io/stream/debug_stream.hpp>
#include <seqan3/offload/serialise/vector.hpp>
#include <seqan3/offload/mpi_offload.hpp>

#include "sum_serial.hpp"

int sum_offload(std::vector<int> numbers)
{
    seqan3::debug_stream << "number of threads: " << std::thread::hardware_concurrency << std::endl;
    seqan3::debug_stream << "called on node " << seqan3::offload::this_node() << '\n';
    seqan3::debug_stream << "vector: " << (numbers | std::ranges::view::take(5)) << "..." << std::endl;
    return sum_serial(numbers);
}

int main(int argc, char **argv)
{
    size_t nodes = seqan3::offload::num_nodes();
    std::cout << "number of nodes: " << nodes << std::endl;

    seqan3::mpi_offload pool{nodes};
    auto ex = pool.executor();

    std::vector<int> numbers{std::ranges::view::iota(0, 100)};
    seqan3::debug_stream << "vector: " << numbers << std::endl;

    seqan3::offload::target_function<sum_offload> fn{1, std::move(numbers)};
    auto future = ex.twoway_execute(fn);

    int s = future.get();
    std::cout << "result of sum[0,...," << numbers.size() << ") = " << s << '\n';
    return 0;
}
