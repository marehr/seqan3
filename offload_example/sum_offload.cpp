
#include <iostream>
#include <numeric>
#include <thread>

#include <seqan3/io/stream/debug_stream.hpp>
#include <seqan3/offload/serialise/vector.hpp>
#include <seqan3/offload/mpi_offload.hpp>

int sum(std::vector<int> numbers)
{
    seqan3::debug_stream << "number of threads: " << std::thread::hardware_concurrency << std::endl;
    seqan3::debug_stream << "called on node " << seqan3::offload::this_node() << '\n';
    seqan3::debug_stream << "vector: " << numbers << std::endl;
    int sum = 0;
    for (auto && k : numbers)
        sum += k;
    return sum;
}

int main(int argc, char **argv)
{
    std::cout << "number of nodes: " << seqan3::offload::num_nodes() << std::endl;

    seqan3::mpi_offload pool{};
    auto ex = pool.executor();

    std::vector<int> numbers{};
    numbers.resize(100);
    std::iota(numbers.begin(), numbers.end(), 0);

    auto result = ex.twoway_execute(seqan3::offload::target_function<sum>{1, std::move(numbers)});
    std::cout << "result of sum[0,...," << numbers.size() << "] = " << result.get() << '\n';
    return 0;
}
