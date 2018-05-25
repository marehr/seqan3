// ============================================================================
//                 SeqAn - The Library for Sequence Analysis
// ============================================================================
//
// Copyright (c) 2006-2017, Knut Reinert & Freie Universitaet Berlin
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
// ============================================================================

/*!\file
 * \author Marcel Ehrhardt <marcel.ehrhardt AT fu-berlin.de>
 * \brief Contains seqan3::offload::target_function
 */

#pragma once

#include <utility>

#include <seqan3/offload/target_migratable.hpp>

namespace seqan3::offload {

template <auto fn_ptr>
struct target_function //: function<fn_ptr>
{
protected:
    template<typename function_ptr_t, function_ptr_t function_ptr>
    struct target_function_traits;

    template<typename result_t, typename... pars_t, result_t (*function_ptr_)(pars_t...)>
    struct target_function_traits<result_t (*)(pars_t...), function_ptr_>
    {
        using result_type = result_t;
        using raw_arguments_type = std::tuple<pars_t...>;
        using arguments_type = std::tuple<target_migratable<pars_t>...>;
        static constexpr auto function_ptr = function_ptr_;
    };

    using traits = target_function_traits<decltype(fn_ptr), fn_ptr>;

public:
    using result_type = typename traits::result_type;
    using raw_arguments_type = typename traits::raw_arguments_type;
    using arguments_type = typename traits::arguments_type;
    static constexpr auto function_ptr = traits::function_ptr;

    template<typename... args_t>
    target_function(node_t target_node_, args_t&&... arguments)
        : target_node{target_node_}, args{{target_node_, std::forward<args_t>(arguments)}...}
    { }

    ham::offload::future<result_type> async()
    {
        return ham::offload::async(target_node, *this);
    }

    // NOTE: this is non-const, since the args which consists of
    // target_migratable's have move semantics; When converting
    // `static_cast<value_t>(target_migratable<value_t>{std::move(args[0])})`
    // the content in target_migratable will be moved out of args[0].
    result_type operator()() // const
    {
        // std::cout << "invoke function_ptr on " << ham::offload::this_node() << std::endl;
        return std::apply(function_ptr, args);
    }

protected:
    node_t target_node;
    arguments_type args;
};

} // seqan3::offload
