// ============================================================================
//                 SeqAn - The Library for Sequence Analysis
// ============================================================================
//
// Copyright (c) 2006-2018, Knut Reinert & Freie Universitaet Berlin
// Copyright (c) 2016-2018, Knut Reinert & MPI Molekulare Genetik
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
 * \brief Provides seqan3::band_static.
 * \author Jörg Winkler <j.winkler AT fu-berlin.de>
 */

#pragma once

#include <seqan3/alignment/band/detail.hpp>
#include <seqan3/core/detail/strong_type.hpp>
#include <seqan3/std/concepts>

namespace seqan3
{

/*!\brief Type for a lower boundary.
 * \ingroup alignment
 * \tparam value_t The underlying type of the lower bound; must be an arithmetic type.
 */
template <typename value_t>
//!\cond
requires std::is_arithmetic_v<value_t>
//!\endcond
struct lower_bound : detail::strong_type<value_t, lower_bound<value_t>>
{
    //!\brief Inheriting constructors from base class.
    using detail::strong_type<value_t, lower_bound<value_t>>::strong_type;
};

/*!\brief Type for an upper boundary.
 * \ingroup alignment
 * \tparam value_t The underlying type of the upper bound; must be an arithmetic type.
 */
template <typename value_t>
//!\cond
requires std::is_arithmetic_v<value_t>
//!\endcond
struct upper_bound : detail::strong_type<value_t, upper_bound<value_t>>
{
    //!\brief Inheriting constructors from base class.
    using detail::strong_type<value_t, upper_bound<value_t>>::strong_type;
};

/*!\name Deduction guides
 * \brief Deduces template parameter from the argument.
 * \{
 */
/*!\brief Deduces the underlying lower boundary type.
 * \relates seqan3::lower_bound
 * \tparam value_t The underlying type of the lower bound; must be an arithmetic type.
 */
template <typename value_t>
//!\cond
requires std::is_arithmetic_v<value_t>
//!\endcond
lower_bound(value_t) -> lower_bound<value_t>;

/*!
 * \brief Deduces the underlying upper boundary type.
 * \relates seqan3::upper_bound
 * \tparam value_t The underlying type of the upper bound; must be an arithmetic type.
 */
template <typename value_t>
//!\cond
requires std::is_arithmetic_v<value_t>
//!\endcond
upper_bound(value_t) -> upper_bound<value_t>;
//!\}


/*!\brief Data structure for a static band.
 * \ingroup alignment
 *
 * \tparam value_t The value type for the diagonal indices; must be integral and unsigned.
 */
template <std::UnsignedIntegral value_t>
struct band_static
{
    /*!\name Constructors, destructor and assignment
     * \{
     */
    band_static()                                = default;
    band_static(band_static const &)             = default;
    band_static(band_static &&)                  = default;
    band_static & operator=(band_static const &) = default;
    band_static & operator=(band_static &&)      = default;
    ~band_static()                               = default;

    /*!\brief Construction from seqan3::lower_bound and seqan3::upper_bound.
     * \tparam input_value_t The input type of the lower and upper diagonal indices.
     * \param lower The lower diagonal that defines the band; must be integral.
     * \param upper The upper diagonal that defines the band; must be integral.
     *
     * \attention The lower diagonal index is a positive number that defines the lower boundary of the band
     * in south direction from the origin. Negative values are converted to their absolute value.
     * Analogously, the upper diagonal index is a positive number that defines the upper boundary of the band
     * in east direction from the origin.
     */
    template <std::Integral input_value_t>
    constexpr band_static(lower_bound<input_value_t> const lower, upper_bound<input_value_t> const upper) noexcept
        : lower_bound{lower.get() >= 0 ? lower.get() : -lower.get()},
          upper_bound{upper.get() >= 0 ? upper.get() : -upper.get()}
    {}
    //!}

    //!\privatesection
    //!\brief The data member storing the lower diagonal index.
    value_t lower_bound{std::numeric_limits<value_t>::max()};
    //!\brief The data member storing the upper diagonal index.
    value_t upper_bound{std::numeric_limits<value_t>::max()};
};

/*!\name Deduction guides
 * \brief Deduces the template parameter from the argument.
 * \relates seqan3::band_static
 * \{
 */
/*!
 * \brief Deduces the band type.
 * \tparam value_t The underlying type of the diagonal indices; must be integral and gets converted to unsigned.
 */
template <std::Integral value_t>
band_static(lower_bound<value_t>, upper_bound<value_t>) -> band_static<typename std::make_unsigned_t<value_t>>;
//!\}

} // namespace seqan3

namespace seqan3::detail
{

//!\cond
template <typename value_t>
struct is_band_config<band_static<value_t>> : public std::true_type
{};
//!\endcond

} // namespace seqan3::detail
