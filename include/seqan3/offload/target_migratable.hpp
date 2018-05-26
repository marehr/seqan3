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
 * \brief Contains seqan3::offload::target_migratable
 */

#pragma once

#include <utility>

#include <seqan3/offload/mpi_backend.hpp>

#include <iostream> // TODO: remove

namespace seqan3::offload
{

template<typename value_t, typename = void>
struct target_migratable
{
    // TODO: remove default constructor when result_container<> uses
    // target_migratable<> internally instead of current implementation
    // result_container<target_migratable<>>
    target_migratable() = default;
    target_migratable(const target_migratable&) = default;
    target_migratable(target_migratable&&) = default;
    target_migratable& operator=(const target_migratable&) = default;
    target_migratable& operator=(target_migratable&&) = default;

    // forward compatible arg into value_t's ctor
    template<typename compatible_t>
    target_migratable(node_t, compatible_t&& arg)
        : value{std::forward<compatible_t>(arg)} // NOTE: compatible types are allowed
    {
        // std::cout << "default-target_migratable-ctor on " << ham::offload::this_node() << " -> " << target_node << ": "; detail::print(value); std::cout << std::endl;
    }

    // NOTE: this is non-const, since it will move out the content
    operator std::decay_t<value_t> ()// const
    {
        // std::cout << "default-target_migratable-conversion on " << ham::offload::this_node() << ": "; detail::print(value); std::cout << std::endl;
        return std::move(value);
    }
protected:
    std::decay_t<value_t> value;
};

} // namespace seqan3::offload
