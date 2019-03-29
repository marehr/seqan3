// ==========================================================================
//                 SeqAn - The Library for Sequence Analysis
// ==========================================================================
//
// Copyright (c) 2006-2017, Knut Reinert, FU Berlin
// Copyright (c) 2016-2017, Knut Reinert & MPI Molekulare Genetik
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Knut Reinert or the FU Berlin nor the names of
//       its contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL KNUT REINERT OR THE FU BERLIN BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// ==========================================================================

#pragma once

#include <seqan3/core/platform.hpp>
#include <seqan3/executors/execution.hpp>
#include <seqan3/executors/future.hpp>
#include <seqan3/offload/target_function.hpp>

namespace seqan3
{

struct mpi_offload
{
protected:
    template<class blocking_t>
    struct executor_impl
    {
        friend struct mpi_offload;

        mpi_offload* offloader;

        friend bool operator==(const executor_impl& a, const executor_impl& b) noexcept
        {
          return a.offloader == b.offloader;
        }

        friend bool operator!=(const executor_impl& a, const executor_impl& b) noexcept
        {
          return a.offloader != b.offloader;
        }

        // Blocking modes.
        template <typename property_t>
        constexpr auto require(property_t) const noexcept
        {
            if constexpr(std::is_same_v<property_t, execution::blocking_t::never_t>)
                return executor_impl<property_t>{};
            else if constexpr(std::is_same_v<property_t, execution::blocking_t::possibly_t>)
                return executor_impl<property_t>{};
            else if constexpr(std::is_same_v<property_t, execution::blocking_t::always_t>)
                return executor_impl<property_t>{};
        }

        template <typename t, typename ...ts>
        static constexpr bool all_same_v = (std::is_same_v<t, ts> && ... && true);

        template <typename property_t>
        constexpr bool query(property_t) const noexcept
        {
            return all_same_v<execution::blocking_t::never_t, property_t, blocking_t> ||
                   all_same_v<execution::blocking_t::always_t, property_t, blocking_t> ||
                   all_same_v<execution::blocking_t::possibly_t, property_t, blocking_t>;
        }

        template<auto fn_ptr>
        auto twoway_execute(offload::function<fn_ptr> function) const
        {
            offload::node_t target_node{offloader->next_node()};
            auto data = std::tuple_cat(std::tuple{target_node}, std::move(function.data()));
            auto target_function = std::make_from_tuple<offload::target_function<fn_ptr>>(std::move(data));

            return target_function.async();
        }

        template<auto fn_ptr>
        auto twoway_execute(offload::target_function<fn_ptr> function) const
        {
            // return ham::offload::async(function.node, function);
            return function.async();
        }

        template<class Function>
        // [[deprecated("mpi_offload::twoway_execute(f): please use seqan3::offload::function as remote function; this invocation will not be offloaded")]]
        auto twoway_execute(Function f) const
        {
            using invoke_type = decltype(f());
            packaged_task<invoke_type()> task(std::move(f));
            future<invoke_type> future = task.get_future();
            task();
            return future;
        }
    };
public:
    using executor_type = executor_impl<execution::blocking_t::possibly_t>;

    mpi_offload(size_t nodes = offload::num_nodes())
    {
        if ((unsigned)nodes > offload::num_nodes())
            nodes = (int)offload::num_nodes();
        // std::cout << "nodes: " << nodes << std::endl;

        this->nodes = nodes;
        current_node = nodes;
    }

    mpi_offload(const mpi_offload &) = delete;
    mpi_offload& operator=(const mpi_offload &) = delete;

    mpi_offload(mpi_offload &&) = default;
    mpi_offload& operator=(mpi_offload &&) = default;

    ~mpi_offload() = default;

    executor_type executor() noexcept
    {
        return executor_type{this};
    }

private:
    seqan3::offload::node_t nodes{};
    seqan3::offload::node_t current_node{};

    seqan3::offload::node_t next_node()
    {
        if (++current_node >= nodes)
            current_node = 1u;
        return current_node;
    }
};

} // seqan3
