#pragma once

#include <seqan3/alphabet/nucleotide/rna4.hpp>

namespace seqan3::literal
{

SEQAN3_API rna4_vector operator""_rna4(const char * s, std::size_t n)
{
    rna4_vector r;
    r.resize(n);

    for (size_t i = 0; i < n; ++i)
        r[i].assign_char(s[i]);

    return r;
}

} // namespace seqan3::literal
