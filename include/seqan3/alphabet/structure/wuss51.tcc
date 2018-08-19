#pragma once

#include <seqan3/alphabet/structure/wuss.hpp>

namespace seqan3::literal
{

SEQAN3_API std::vector<wuss51> operator""_wuss51(const char * s, std::size_t n)
{
    std::vector<wuss51> r;
    r.resize(n);

    for (size_t i = 0; i < n; ++i)
        r[i].assign_char(s[i]);

    return r;
}

} // namespace seqan3::literal
