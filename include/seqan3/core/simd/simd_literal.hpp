
#pragma once

#include <seqan3/core/simd/concept.hpp>
#include <seqan3/core/simd/simd_traits.hpp>

namespace seqan3::detail
{

template <auto v>
struct simd_literal
{
    template <auto v2>
    constexpr bool operator==(simd_literal<v2> const &) const noexcept
    {
        return v == v2;
    }

    template <auto v2>
    constexpr bool operator!=(simd_literal<v2> const &) const noexcept
    {
        return v != v2;
    }

    template <simd_concept simd_t>
    constexpr operator simd_t() const noexcept
    {
        return simd_t{};
    }
};

}

namespace seqan3
{

inline namespace simd
{


} // inline namespace simd

} // namespace seqan3
