#include <gtest/gtest.h>

#include <seqan3/core/simd/simd.hpp>
#include <seqan3/core/simd/simd_literal.hpp>
#include <seqan3/std/concepts>
#include <seqan3/test/simd_utility.hpp>

using namespace seqan3;

TEST(simd_literal, equal)
{
    // detail::builtin_simd_traits_helper<int>;
    detail::simd_literal<4> four{};
    detail::simd_literal<5> five{};

    EXPECT_EQ(four, four);
    EXPECT_EQ(five, five);
    EXPECT_FALSE(four == five);
    EXPECT_FALSE(five == four);
}

TEST(simd_literal, unequal)
{
    detail::simd_literal<4> four{};
    detail::simd_literal<5> five{};

    EXPECT_FALSE(four != four);
    EXPECT_FALSE(five != five);
    EXPECT_NE(four, five);
    EXPECT_NE(five, four);
}

// TEST(simd_literal, regular_type)
// {
//     EXPECT_TRUE((std::Regular<detail::simd_literal<5>>));
// }
//
// TEST(simd_literal, implict_convertible)
// {
//     // using simd_type = simd::simd_type_t<uint16_t, 8>;
//     // // constexpr simd_type result = detail::simd_literal<5>{};
//     // simd_type expect = simd::fill<simd_type>(5);
//     //
//     // // SIMD_EQ(expect, result);
// }

// TEST(simd_literal, literal)
// {
//     detail::simd_literal<5> five = 5_simd;
//     EXPECT_EQ(five, 5_simd);
// }
