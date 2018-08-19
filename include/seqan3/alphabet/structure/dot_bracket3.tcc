#pragma once

#include <seqan3/alphabet/structure/dot_bracket3.hpp>

namespace seqan3::literal
{

SEQAN3_API std::vector<dot_bracket3> operator""_db3(const char * s, std::size_t n)
{
    std::vector<dot_bracket3> r;
    r.resize(n);

    for (size_t i = 0; i < n; ++i)
        r[i].assign_char(s[i]);

    return r;
}

} // namespace seqan3::literal
