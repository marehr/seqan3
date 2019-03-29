#pragma once

#include <seqan3/io/stream/debug_stream.hpp>
#include <seqan3/range/view/slice.hpp>
#include <seqan3/std/ranges>
#include <experimental/thread_pool>

#include "sum_serial.hpp"
#include "trim.hpp"

std::mutex io_mutex;

void debug(std::span<int> numbers)
{
    std::lock_guard lock(io_mutex);
    seqan3::debug_stream << "on thread: " << '\n';
    seqan3::debug_stream << "id: " << std::this_thread::get_id() << std::endl;
    seqan3::debug_stream << "numbers: " << trim(numbers) << std::endl << std::endl;
}

template <auto sum_fn>
int sum_thread(std::span<int> numbers, size_t threads)
{
    size_t blocks = threads;
    seqan3::debug_stream << "number of threads: " << threads << std::endl << std::endl;

    auto partial_sum_fn = [blocks](int i,
                                   std::vector<int> & partial_sums,
                                   std::span<int> & numbers)
    {
        size_t block_size = (numbers.size() / blocks) + 1;
        auto numbers_block = numbers | seqan3::view::slice(i * block_size, (i + 1)*block_size);
        debug(numbers_block);

        partial_sums[i] = sum_fn(numbers_block);
    };

    std::experimental::static_thread_pool pool{threads};
    auto ex = pool.executor();
    auto result = ex.bulk_twoway_execute(partial_sum_fn, blocks, // execute blocks many times sum_fn
                                         [blocks]()
                                         {
                                             return std::vector<int>(blocks);
                                         }, // shared result between threads
                                         [numbers]() -> std::span<int>
                                         {
                                             return numbers;
                                         }); // shared state between threads

    std::vector<int> partial_sums{result.get()};
    seqan3::debug_stream << "partial_sums[...]: " << partial_sums << std::endl;
    return sum_fn(partial_sums);
}
