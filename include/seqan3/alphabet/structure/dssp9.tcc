#pragma once

#include <seqan3/alphabet/structure/dssp9.hpp>

namespace seqan3::literal
{

SEQAN3_API std::vector<dssp9> operator""_dssp9(const char * s, std::size_t n)
{
    std::vector<dssp9> r;
    r.resize(n);

    for (size_t i = 0; i < n; ++i)
        r[i].assign_char(s[i]);

    return r;
}

} // namespace seqan3::literal
