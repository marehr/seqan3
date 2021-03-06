#include <sstream>
#include <string>
#include <vector>

#include <seqan3/alphabet/nucleotide/rna5.hpp>
#include <seqan3/alphabet/structure/wuss.hpp>
#include <seqan3/io/structure_file/output.hpp>

int main()
{
    using seqan3::operator""_rna5;
    using seqan3::operator""_wuss51;

    seqan3::structure_file_output fout{std::ostringstream{}, seqan3::format_vienna{}};

    for (int i = 0; i < 10; i++) // ...
    {
        std::string id{"test_id"};
        seqan3::rna5_vector seq{"AGGGUU"_rna5};
        std::vector<seqan3::wuss51> structure{"..().."_wuss51};

        // ...

        fout.emplace_back(seq, id, structure);
    }
}
