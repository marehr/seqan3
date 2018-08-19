#pragma once

#include <seqan3/alphabet/nucleotide/dna5.hpp>

namespace seqan3::literal
{

SEQAN3_API dna5_vector operator""_dna5(const char * s, std::size_t n)
{
    dna5_vector r;
    r.resize(n);

    for (size_t i = 0; i < n; ++i)
        r[i].assign_char(s[i]);

    return r;
}

} // namespace seqan3::literal
