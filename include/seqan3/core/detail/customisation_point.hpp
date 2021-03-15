// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2020, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2020, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \author Hannes Hauswedell <hannes.hauswedell AT fu-berlin.de>
 * \brief Helper utilities for defining customisation point objects (CPOs).
 */

#pragma once

#include <seqan3/core/platform.hpp>

namespace seqan3::detail
{

// ============================================================================
// priority_tag
// ============================================================================

//!\brief A tag that allows controlled overload resolution via implicit base conversion rules.
template <size_t I>
struct priority_tag
//!\cond
// Doxygen fail
: priority_tag<I-1>
//!\endcond
{};

//!\brief Recursion anchor for seqan3::detail::priority_tag.
template <>
struct priority_tag<0>
{};

} // seqan3::detail

// ============================================================================
// SEQAN3_CPO_IMPL
// ============================================================================

//!\brief A macro that helps defining the overload set of a customisation point.
//!\deprecated Please use #SEQAN3_CPO_OVERLOAD to define seqan3::detail::customisation_point_object instead.
#ifdef SEQAN3_DEPRECATED_310
#define SEQAN3_CPO_IMPL(PRIO, TERM)                                                                                  \
/*!\brief A customisation point overload.*/                                                                          \
template <typename t, typename ...arg_ts>                                                                            \
static constexpr decltype(auto) impl(seqan3::detail::priority_tag<PRIO>,                                             \
                                     [[maybe_unused]] t && v,                                                        \
                                     [[maybe_unused]] arg_ts && ... args)                                            \
    noexcept(noexcept(TERM))                                                                                         \
    requires requires (seqan3::detail::priority_tag<PRIO> const &/*<- need for doxygen*/, t && v, arg_ts && ... args)\
    { { TERM }; }                                                                                                    \
{                                                                                                                    \
    return TERM;                                                                                                     \
}
#endif // SEQAN3_DEPRECATED_310

#if SEQAN3_DOXYGEN_ONLY(1)0
//!\brief A macro helper for #SEQAN3_CPO_OVERLOAD.
#define SEQAN3_CPO_OVERLOAD_BODY(...) noexcept(auto) { return __VA_ARGS__; }
#else // ^^^ (simplified) doxygen version / normal definition vvv
#define SEQAN3_CPO_OVERLOAD_BODY(...) \
        noexcept(noexcept(__VA_ARGS__)) \
        -> decltype(__VA_ARGS__) \
    { return __VA_ARGS__; } \
    static_assert(true)
#endif

/*!\brief A macro that helps to define a seqan3::detail::customisation_point_object.
 * \details
 * Expands to a function definition with the name `cpo_overload`, a puts the given expression via
 * #SEQAN3_CPO_OVERLOAD_BODY as a single return statement in the function body, the noexcept declaration and requires
 * declaration.
 *
 * ```cpp
 * template <typename alphabet_t>
 * static constexpr auto SEQAN3_CPO_OVERLOAD(seqan3::detail::priority_tag<2>,
 *                                           seqan3::alphabet_rank_t<alphabet_t> const rank,
 *                                           alphabet_t && alphabet)
 * (
 *     seqan3::custom::alphabet<alphabet_t>::assign_rank_to(rank, alphabet)
 * );
 * ```
 *
 * expands to something similar to
 *
 * ```cpp
 * template <typename alphabet_t>
 * static constexpr auto cpo_overload(seqan3::detail::priority_tag<2>,
 *                                    seqan3::alphabet_rank_t<alphabet_t> const rank,
 *                                    alphabet_t && alphabet)
 *     noexcept(noexcept(seqan3::custom::alphabet<alphabet_t>::assign_rank_to(rank, alphabet)))
 *     requires requires() { {seqan3::custom::alphabet<alphabet_t>::assign_rank_to(rank, alphabet)}; }
 * {
 *     return seqan3::custom::alphabet<alphabet_t>::assign_rank_to(rank, alphabet);
 * }
 * ```
 */
#define SEQAN3_CPO_OVERLOAD(...) \
    cpo_overload(__VA_ARGS__) \
    SEQAN3_CPO_OVERLOAD_BODY

namespace seqan3::detail
{
/*!\brief A CRTP base-class that defines a customisation_point_object (CPO).
 * \tparam derived_t CRTP derived type.
 * \tparam max_priority The (highest) start priority the cpo overload's should be checked from.
 */
template <typename derived_t, unsigned max_priority>
struct customisation_point_object
{
private:
    //!\brief Allow `derived_t` to inherit the constructors of this CRTP-class.
    friend derived_t;

    constexpr customisation_point_object() = default; //!< Defaulted.
    constexpr customisation_point_object(customisation_point_object &&) = default; //!< Defaulted.
    constexpr customisation_point_object(customisation_point_object const &) = default; //!< Defaulted.
    constexpr customisation_point_object & operator=(customisation_point_object &&) = default; //!< Defaulted.
    constexpr customisation_point_object & operator=(customisation_point_object const &) = default; //!< Defaulted.

public:
    /*!\brief SFINAE-friendly call-operator of this seqan3::detail::customisation_point_object instance.
     * \details
     * This operator implements the actual CPO overload resolution. It starts the with
     * seqan3::detail::priority_tag<`max_priority`> `derived_t::cpo_overload` and tries each overload until it reaches
     * seqan3::detail::priority_tag<0>. That means a high priority number is evaluated first.
     *
     * It perfectly forwards the result and noexcept-property of the `derived_t::cpo_overload`.
     */
    template <typename ...args_t, typename derived_type = derived_t /*circumvent incomplete types*/>
    constexpr auto operator()(args_t && ...args) const
    SEQAN3_CPO_OVERLOAD_BODY
    (
        /*return*/ derived_type::cpo_overload(priority_tag<max_priority>{}, std::forward<args_t>(args)...) /*;*/
    );
};
} // namespace seqan3::detail
