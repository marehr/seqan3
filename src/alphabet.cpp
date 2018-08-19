#define SEQAN3_API
#define SEQAN3_EXTERN_TEMPLATE 1

#include <seqan3/alphabet/aminoacid/all.hpp>
#include <seqan3/alphabet/gap/all.hpp>
#include <seqan3/alphabet/nucleotide/all.hpp>
#include <seqan3/alphabet/quality/all.hpp>
#include <seqan3/alphabet/structure/all.hpp>

namespace std
{
template class vector<seqan3::aa20>;
template class vector<seqan3::aa27>;

template class vector<seqan3::dna4>;
template class vector<seqan3::dna5>;
template class vector<seqan3::dna15>;

template class vector<seqan3::rna4>;
template class vector<seqan3::rna5>;
template class vector<seqan3::rna15>;

template class vector<seqan3::dot_bracket3>;
template class vector<seqan3::dssp9>;
template class vector<seqan3::wuss51>;
}

namespace seqan3
{
template std::ostream & operator<< <dna4>(std::ostream & os, dna4 const alph);
template char to_char<dna4>(dna4 const alph);
template uint8_t to_rank<dna4>(dna4 const alph);
template dna4 & assign_char<dna4>(dna4 & alph, char const chr);
template dna4 && assign_char<dna4>(dna4 && alph, char const chr);
template dna4 & assign_rank<dna4>(dna4 & alph, uint8_t const chr);
template dna4 && assign_rank<dna4>(dna4 && alph, uint8_t const chr);

template std::ostream & operator<< <dna5>(std::ostream & os, dna5 const alph);
template char to_char<dna5>(dna5 const alph);
template uint8_t to_rank<dna5>(dna5 const alph);
template dna5 & assign_char<dna5>(dna5 & alph, char const chr);
template dna5 && assign_char<dna5>(dna5 && alph, char const chr);
template dna5 & assign_rank<dna5>(dna5 & alph, uint8_t const chr);
template dna5 && assign_rank<dna5>(dna5 && alph, uint8_t const chr);

template std::ostream & operator<< <dna15>(std::ostream & os, dna15 const alph);
template char to_char<dna15>(dna15 const alph);
template uint8_t to_rank<dna15>(dna15 const alph);
template dna15 & assign_char<dna15>(dna15 & alph, char const chr);
template dna15 && assign_char<dna15>(dna15 && alph, char const chr);
template dna15 & assign_rank<dna15>(dna15 & alph, uint8_t const chr);
template dna15 && assign_rank<dna15>(dna15 && alph, uint8_t const chr);

template std::ostream & operator<< <rna4>(std::ostream & os, rna4 const alph);
template char to_char<rna4>(rna4 const alph);
template uint8_t to_rank<rna4>(rna4 const alph);
template rna4 & assign_char<rna4>(rna4 & alph, char const chr);
template rna4 && assign_char<rna4>(rna4 && alph, char const chr);
template rna4 & assign_rank<rna4>(rna4 & alph, uint8_t const chr);
template rna4 && assign_rank<rna4>(rna4 && alph, uint8_t const chr);

template std::ostream & operator<< <rna5>(std::ostream & os, rna5 const alph);
template char to_char<rna5>(rna5 const alph);
template uint8_t to_rank<rna5>(rna5 const alph);
template rna5 & assign_char<rna5>(rna5 & alph, char const chr);
template rna5 && assign_char<rna5>(rna5 && alph, char const chr);
template rna5 & assign_rank<rna5>(rna5 & alph, uint8_t const chr);
template rna5 && assign_rank<rna5>(rna5 && alph, uint8_t const chr);

template std::ostream & operator<< <rna15>(std::ostream & os, rna15 const alph);
template char to_char<rna15>(rna15 const alph);
template uint8_t to_rank<rna15>(rna15 const alph);
template rna15 & assign_char<rna15>(rna15 & alph, char const chr);
template rna15 && assign_char<rna15>(rna15 && alph, char const chr);
template rna15 & assign_rank<rna15>(rna15 & alph, uint8_t const chr);
template rna15 && assign_rank<rna15>(rna15 && alph, uint8_t const chr);

template std::ostream & operator<< <aa20>(std::ostream & os, aa20 const alph);
template char to_char<aa20>(aa20 const alph);
template uint8_t to_rank<aa20>(aa20 const alph);
template aa20 & assign_char<aa20>(aa20 & alph, char const chr);
template aa20 && assign_char<aa20>(aa20 && alph, char const chr);
template aa20 & assign_rank<aa20>(aa20 & alph, uint8_t const chr);
template aa20 && assign_rank<aa20>(aa20 && alph, uint8_t const chr);

template std::ostream & operator<< <aa27>(std::ostream & os, aa27 const alph);
template char to_char<aa27>(aa27 const alph);
template uint8_t to_rank<aa27>(aa27 const alph);
template aa27 & assign_char<aa27>(aa27 & alph, char const chr);
template aa27 && assign_char<aa27>(aa27 && alph, char const chr);
template aa27 & assign_rank<aa27>(aa27 & alph, uint8_t const chr);
template aa27 && assign_rank<aa27>(aa27 && alph, uint8_t const chr);

template std::ostream & operator<< <dot_bracket3>(std::ostream & os, dot_bracket3 const alph);
template char to_char<dot_bracket3>(dot_bracket3 const alph);
template uint8_t to_rank<dot_bracket3>(dot_bracket3 const alph);
template dot_bracket3 & assign_char<dot_bracket3>(dot_bracket3 & alph, char const chr);
template dot_bracket3 && assign_char<dot_bracket3>(dot_bracket3 && alph, char const chr);
template dot_bracket3 & assign_rank<dot_bracket3>(dot_bracket3 & alph, uint8_t const chr);
template dot_bracket3 && assign_rank<dot_bracket3>(dot_bracket3 && alph, uint8_t const chr);

template std::ostream & operator<< <wuss51>(std::ostream & os, wuss51 const alph);
template char to_char<wuss51>(wuss51 const alph);
template uint8_t to_rank<wuss51>(wuss51 const alph);
template wuss51 & assign_char<wuss51>(wuss51 & alph, char const chr);
template wuss51 && assign_char<wuss51>(wuss51 && alph, char const chr);
template wuss51 & assign_rank<wuss51>(wuss51 & alph, uint8_t const chr);
template wuss51 && assign_rank<wuss51>(wuss51 && alph, uint8_t const chr);

template std::ostream & operator<< <dssp9>(std::ostream & os, dssp9 const alph);
template char to_char<dssp9>(dssp9 const alph);
template uint8_t to_rank<dssp9>(dssp9 const alph);
template dssp9 & assign_char<dssp9>(dssp9 & alph, char const chr);
template dssp9 && assign_char<dssp9>(dssp9 && alph, char const chr);
template dssp9 & assign_rank<dssp9>(dssp9 & alph, uint8_t const chr);
template dssp9 && assign_rank<dssp9>(dssp9 && alph, uint8_t const chr);

template class seqan3::wuss<51>;

template class union_composition<dna4, gap>;
template class gapped<dna4>;
template class union_composition<dna5, gap>;
template class gapped<dna5>;
template class union_composition<dna15, gap>;
template class gapped<dna15>;
template class union_composition<rna4, gap>;
template class gapped<rna4>;
template class union_composition<rna5, gap>;
template class gapped<rna5>;
template class union_composition<rna15, gap>;
template class gapped<rna15>;

template class cartesian_composition<qualified<dna4, phred42>, dna4, phred42>;
template class qualified<dna4, phred42>;
template class cartesian_composition<qualified<dna5, phred42>, dna5, phred42>;
template class qualified<dna5, phred42>;
template class cartesian_composition<qualified<dna15, phred42>, dna15, phred42>;
template class qualified<dna15, phred42>;
template class cartesian_composition<qualified<rna4, phred42>, rna4, phred42>;
template class qualified<rna4, phred42>;
template class cartesian_composition<qualified<rna5, phred42>, rna5, phred42>;
template class qualified<rna5, phred42>;
template class cartesian_composition<qualified<rna15, phred42>, rna15, phred42>;
template class qualified<rna15, phred42>;

template class cartesian_composition<structured_aa<aa27, dssp9>, aa27, dssp9>;
template class structured_aa<aa27, dssp9>;

template class cartesian_composition<structured_rna<rna4, dot_bracket3>, rna4, dot_bracket3>;
template class structured_rna<rna4, dot_bracket3>;
} // namespace seqan3
