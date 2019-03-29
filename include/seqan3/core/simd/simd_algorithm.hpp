// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \brief Contains algorithms to modify seqan3::simd::simd_type.
 * \author Marcel Ehrhardt <marcel.ehrhardt AT fu-berlin.de>
 */

#pragma once

#include <utility>

#include <seqan3/core/simd/concept.hpp>
#include <seqan3/core/simd/simd_traits.hpp>

namespace seqan3::detail
{

//!\brief Helper function for seqan3::detail::simd_transform_constexpr.
//!\ingroup simd
template <typename simd_output_t, typename operation_t, typename ...simds_t, typename scalar_t, scalar_t... is>
constexpr simd_output_t simd_transform_constexpr_impl(std::integer_sequence<scalar_t, is...>, operation_t operation, simds_t const & ...simds)
{
    using scalar_type = scalar_t;

    return simd_output_t
    {
        [&](scalar_type i) -> scalar_type
        {
            return operation(i, simds[i]...);
        }(is)...
    };
}

//!\copydoc seqan3::detail::simd_transform
template <simd_concept simd_output_t, typename operation_t, simd_concept ...simds_t>
//!\cond
    requires std::Invocable<operation_t, typename simd_traits<simd_output_t>::scalar_type const, typename simd_traits<simds_t>::scalar_type const ...> &&
             ((simd_traits<simd_output_t>::length <= simd_traits<simds_t>::length) && ... && true)
//!\endcond
constexpr simd_output_t simd_transform_constexpr(operation_t operation, simds_t const & ...simds)
{
    using scalar_type = typename simd_traits<simd_output_t>::scalar_type;
    constexpr auto length = simd_traits<simd_output_t>::length;

    return simd_transform_constexpr_impl<simd_output_t>(std::make_integer_sequence<scalar_type, length>{},
                                                        std::forward<operation_t>(operation),
                                                        simds...);
}

/*!\brief This applies a given function to given simd vectors and stores the result in another simd vector.
 * \ingroup simd
 *
 * \tparam simd_output_t The output simd vector, must satisfy seqan3::simd::simd_concept.
 * \tparam operation_t   The function to apply, must be std::Invocable.
 * \tparam simds_t       The input simd vectors (can also be empty), must satisfy seqan3::simd::simd_concept and the
 *                       length of each input simd vector must be at least the length of the output simd vector.
 *
 * \param  operation     The function to apply.
 * \param  simds         Input simd vectors.
 * \return A new simd vector will be constructed by applying the function element-wise.
 *
 * \details
 *
 * The definition of seqan3::detail::simd_transform:
 *
 * \snippet test/snippet/core/simd/detail/simd_transform.cpp definition
 *
 * You can use it as a generator:
 *
 * \snippet test/snippet/core/simd/detail/simd_transform.cpp generator
 *
 * Or as a binary function:
 *
 * \snippet test/snippet/core/simd/detail/simd_transform.cpp binary_max
 *
 * \attention Note that depending on the given function the seqan3::detail::simd_transform might not be simdified by the
 * compiler and therefore executed scalar which can result in a huge performance impact. We figured out that a lot of
 * simple functions, like min, max, abs, blend, will be auto-vectorized (e.g. translated into the correct cpu
 * instruction) by the compiler. We tested that our simd algorithms which use seqan3::detail::simd_transform will always
 * produce the most specific and correct simd instruction for a given platform (either by the compiler or by explicit
 * simd instructions).
 *
 * \attention The operation_t function must not modify any elements of the input `simds` (Same behaviour as in
 * std::transform).
 */
template <simd_concept simd_output_t, typename operation_t, simd_concept ...simds_t>
//!\cond
    requires std::Invocable<operation_t, typename simd_traits<simd_output_t>::scalar_type const, typename simd_traits<simds_t>::scalar_type const ...> &&
             ((simd_traits<simd_output_t>::length <= simd_traits<simds_t>::length) && ... && true)
//!\endcond
inline simd_output_t simd_transform(operation_t operation, simds_t const & ...simds)
{
    using scalar_type = typename simd_traits<simd_output_t>::scalar_type;
    constexpr auto length = simd_traits<simd_output_t>::length;

    simd_output_t result{};
    // #pragma omp simd
    for (scalar_type i = 0; i < length; ++i)
        result[i] = operation(i, simds[i]...);

    return result;
}

} // namespace seqan3::detail

namespace seqan3
{

inline namespace simd
{

/*!\brief Fills a seqan3::simd::simd_type vector with a scalar value.
 * \tparam    simd_t The simd type which satisfies seqan3::simd::simd_concept.
 * \param[in] scalar The scalar value to fill the seqan3::simd::simd_type vector.
 * \ingroup simd
 *
 * \details
 *
 * \include test/snippet/core/simd/fill.cpp
 */
template <simd_concept simd_t>
constexpr simd_t fill(typename simd_traits<simd_t>::scalar_type const scalar)
{
    using scalar_type = typename simd_traits<simd_t>::scalar_type;
    // gcc will produce `vpbroadcastd` in non-constexpr case; clang is unable to auto-vectorize this
    return detail::simd_transform_constexpr<simd_t>([scalar] (auto) -> scalar_type
    {
        return scalar;
    });
}

/*!\brief Fills a seqan3::simd::simd_type vector with the scalar values offset, offset+1, offset+2, ...
 * \tparam    simd_t The simd type which satisfies seqan3::simd::simd_concept.
 * \param[in] offset The scalar offset to fill the seqan3::simd::simd_type vector.
 * \ingroup simd
 *
 * \details
 *
 * \include test/snippet/core/simd/iota.cpp
 */
template <simd_concept simd_t>
constexpr simd_t iota(typename simd_traits<simd_t>::scalar_type const offset)
{
    using scalar_type = typename simd_traits<simd_t>::scalar_type;
    // gcc will produce iota = simd::fill(offset) + constexpr iota(0, 1, 2, ..., length-1) in non-constexpr case;
    // that means gcc will produce `vpbroadcastd` and `paddd` in non-constexpr case
    // clang is unable to auto-vectorize this
    return detail::simd_transform_constexpr<simd_t>([offset] (scalar_type const i) -> scalar_type
    {
        return offset + i;
    });
}

template <simd_concept simd_t>
constexpr simd_t blend(typename simd_traits<simd_t>::mask_type const & m, simd_t const & a, simd_t const & b)
{
    return m ? a : b;
}

template <simd_concept simd_t>
constexpr simd_t max(simd_t const & a, simd_t const & b)
{
    return blend(a > b, a, b);
}

template <simd_concept simd_t>
constexpr simd_t min(simd_t const & a, simd_t const & b)
{
    return blend(a < b, a, b);
}

template <typename scalar_t, simd_concept simd_t>
constexpr void store(scalar_t * const memory, simd_t const & a)
{
    for (size_t i = 0; i < simd_traits<simd_t>::length; ++i)
        memory[i] = a[i];
}

template <simd_concept simd_t, typename scalar_t>
constexpr simd_t load(scalar_t const * const memory)
{
    simd_t output{};
    for (size_t i = 0; i < simd_traits<simd_t>::length; ++i)
        output[i] = memory[i];
    return output;
}

} // inline namespace simd

} // namespace seqan3
