#pragma once

#include <seqan3/alphabet/nucleotide/rna15.hpp>

namespace seqan3::literal
{

SEQAN3_API rna15_vector operator""_rna15(const char * s, std::size_t n)
{
    rna15_vector r;
    r.resize(n);

    for (size_t i = 0; i < n; ++i)
        r[i].assign_char(s[i]);

    return r;
}

} // namespace seqan3::literal
