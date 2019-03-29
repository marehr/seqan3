#pragma once

#include <vector>

#include <seqan3/offload/serialise/vector.hpp>
#include <seqan3/offload/mpi_offload.hpp>
#include <seqan3/range/view/slice.hpp>
#include <seqan3/std/span>

#include "sum_serial.hpp"
#include "trim.hpp"

int sum_offload_node(std::vector<int>);

int sum_offload_host(std::span<int> numbers)
{
    size_t nodes = seqan3::offload::num_nodes();
    size_t blocks = nodes - 1;
    size_t block_size = (numbers.size() / blocks) + 1;

    seqan3::mpi_offload pool{nodes};

    using sum_fn_t = seqan3::offload::function<sum_offload_node>;
    using result_t = decltype(pool.executor().twoway_execute(sum_fn_t{numbers}));

    std::vector<result_t> barriers(blocks);
    for (int i = 0; i < blocks; ++i)
    {
        auto numbers_block = numbers | seqan3::view::slice(i * block_size, (i+1)*block_size);
        std::vector<int> numbers_block_v(std::begin(numbers_block), std::end(numbers_block));

        sum_fn_t sum_fn{numbers_block_v};
        auto future = pool.executor().twoway_execute(std::move(sum_fn));
        barriers[i] = std::move(future);
    }

    std::vector<int> partial_sums(blocks);
    for (int i = 0u; i < blocks; ++i)
        partial_sums[i] = barriers[i].get();

    return sum_serial(partial_sums);
}
