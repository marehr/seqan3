
#include <iostream>
#include <seqan3/offload/mpi_offload.hpp>

int add(int a, int b)
{
    std::cout << "called on node " << seqan3::offload::this_node() << '\n';
    return a + b;
}

int main(int argc, char **argv)
{
    std::cout << "this is node " << seqan3::offload::this_node() << '\n';
    std::cout << "this is host_node " << (seqan3::offload::is_host() ? "true" : "false") << '\n';
    std::cout << "number of nodes: " << seqan3::offload::num_nodes() << std::endl;

    seqan3::mpi_offload pool{};
    auto ex = pool.executor();

    auto result = ex.twoway_execute(seqan3::offload::function<add>{15, 12});
    std::cout << "result of 15 + 12 = " << result.get() << '\n';
    return 0;
}
