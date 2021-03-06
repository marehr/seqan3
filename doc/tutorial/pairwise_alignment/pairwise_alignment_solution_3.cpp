#include <utility>

#include <seqan3/alignment/pairwise/align_pairwise.hpp>
#include <seqan3/alignment/scoring/aminoacid_scoring_scheme.hpp>
#include <seqan3/alignment/scoring/gap_scheme.hpp>
#include <seqan3/alphabet/aminoacid/aa27.hpp>
#include <seqan3/core/debug_stream.hpp>

using seqan3::operator""_aa27;

int main()
{
    auto seq1 = "QFSEEILSDIYCWMLQCGQERAV"_aa27;
    auto seq2 = "AFLPGWQEENKLSKIWMKDCGCLW"_aa27;

    // Configure the alignment kernel.
    auto config = seqan3::align_cfg::mode{seqan3::global_alignment} |
                  seqan3::align_cfg::scoring{seqan3::aminoacid_scoring_scheme{
                      seqan3::aminoacid_similarity_matrix::BLOSUM62}} |
                  seqan3::align_cfg::gap{seqan3::gap_scheme{seqan3::gap_score{-2}, seqan3::gap_open_score{-9}}};

    for (auto const & res : seqan3::align_pairwise(std::tie(seq1, seq2), config))
        seqan3::debug_stream << "Score: " << res.score() << '\n';
}
