
#pragma once

#include <seqan3/alphabet/aminoacid/aa20.hpp>

namespace seqan3::literal
{

SEQAN3_API aa20_vector operator""_aa20(const char * s, std::size_t n)
{
    aa20_vector r;
    r.resize(n);

    for (size_t i = 0; i < n; ++i)
        r[i].assign_char(s[i]);

    return r;
}

} // namespace seqan3::literal
