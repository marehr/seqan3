
// ================== HEADER start sequence_file_input_default.hpp ==================

#include <format_fasta>
#include <format_fastq>
#include <dna5>
#include <phred42>

class sequence_default_record
{

    std::string id()
    {
        return record.id;
    }

    std::vector<seqan3::dna5> sequence()
    {
        return record.sequence;
    }

    std::vector<seqan3::phred42> quality()
    {
        return record.quality;
    }
};

formats default_sequence_file_input_formats ()
{
    return formats;
}

// forward declare // or include sequence_file_input.hpp
template <typename record>
class sequence_file_input;

// CTAD
sequence_file_input(std::filesystem::path const file_path, formats_t && ...formats)
    -> sequence_file_input<sequence_default_record>; // could be done in an extra include;

// ================== HEADER end sequence_file_input_default.hpp ==================

// ================== How to implement format_converter_cpo - start ==================

class format_converter_cpo
{

    // fasta_record_native to sequence_record_native
    template <>
    void operator()<fasta_record_native, sequence_record_native>()
    {
        // memory_location, parse_projection: [](std::span<char> & memory) -> ???
        id // span<istream>
        sequence // std::segmented_span<const char>? (something, to_char_range) -> range<char> -> range<alphabet>
        quality // std::segmented_span<const char>?
        fasta_native.raw_sequence() -> type_erased_alphabet_sequence_over_char{fasta_record_native.sequence_memory, fasta_record_native.filter_sequence};
        // could be span of sequence_memory or segmented span

        sequence_record_native.sequence // should be segmented memory_location if possible

        sequence_record_native.sequence = fasta_native.raw_sequence() // could this be an any_range concept? inheritance virtual pattern design


        // fasta_native
    }
};

// ================== HEADER end format_converter_cpo - end ==================

// ================== HEADER start sequence_file_input.hpp ==================

#include <filesystem>

// no includes into alphabet / formats
template <typename record_t>
class sequence_file_input
{

    // Option c)
    template <typename ...formats_t>
    sequence_file_input(std::filesystem::path const file_path, formats_t && ...formats)
    {
        // Use default: format_fasta{}, format_fastq{} // by default_sequence_file_input_formats();
        // select format from formats and file_path.extension();
        format = select(file_path, formats...);
    }


    type_erased_format format{};

    template <>
    struct type_erased_format<fasta_format>
    {
       virtual sequence_record_native read_next(istream)
       {
           using record_t = fasta_record_native; //typename specific_format_t::record_type;

           fasta_record_native record = _format.read_next(istream);
           return sequence_record_native{record};
           // could mean:
           //
           // sequence_record_native has constructor for fasta_record_native
           // cons:
           //   - bad
           //   - not decoupled
           //
           // fasta_record_native has conversion operator for sequence_record_native (semi-good)
           // pros:
           //   - decoupled;
           // cons:
           //   - fasta_record_native must know sequence_record_native -> more include dependencies (might be solved by forward declaration)
           //
           // global conversion operator? (is this good?)
           // pros:
           //   - decoupled
           //   - include dependencies only what is needed
           //   - mechanism to make any format produce a sequence record is very flexible
           // cons:
           //   - correct use might be harder (you need to know what to include)
           //

           // like global conversion operator but with explicit name
           // could be postponed to after seqan3.1
           return format_converter_cpo(sequence_record_native, record); // -> could try sequence_record_native{record} and then ::format_converter(sequence_record_native, record)
       };

       specific_format_t _format{};
    };

    iterator
    {
        record_t cached_record{};

        void ++operator()
        {
           sequence_record_native format.read_next(istream); // what does this return?
        }
    };

};

// ================== HEADER end sequence_file_input.hpp ==================

// ------------------------------------------------------------------------
// Use Case 1: The most elegant, shortest default case.
// ------------------------------------------------------------------------

#include <seqan3/io/sequence_file/all.hpp>
#include <seqan3/core/debug_stream.hpp>

using namespace seqan3;

int main()
{
    // The most elegant, shortest default case.
    for (auto & record : sequence_file_input{"my_file.[fq,fa]"}}) // OPEN: How is default record_type?
    {
        debug_stream << "id: " << record.id() << '\n'
                     << "sequence: " << record.sequence() << '\n'
                     << "quality: " << record.quality() << '\n';
    }

}

// ------------------------------------------------------------------------
// Use Case 2: Change the alphabet default parameter to seqan3::aa27 and truncate id. (id until first space)
// ------------------------------------------------------------------------

#include <seqan3/io/sequence_file/all.hpp>
#include <seqan3/core/debug_stream.hpp>

using namespace seqan3;

int main()
{
    // ------------------------------------------------------------------------
    // Select aa27 as alphabet
    // ------------------------------------------------------------------------

    // case 1:
    std::vector<seqan3::aa27> amino_seq{};
    for (auto && record : sequence_file_input{"my_file.fq"}})
    {
        record.sequence(amino_seq);// Amino acid sequence;
        record.sequence<std::vector<seqan3::aa27>>();
        record.template sequence<std::vector<seqan3::aa27>>(); // requires explicit template argument in generic context
    }

    // case 2:
    template <typename alph_t>
    class sequence_record_type
    {
        std::vector<alph_t> sequence();
    };

    template <typename alph_t>
    using sequence_file_input = sequence_file_base<sequence_record_type<alph_t>>;

    for (auto && record : sequence_file_input<sequence_record_type<alph_t>>{"my_file.fq"}})

    for (auto && record : sequence_file_input<seqan3::aa27>{"my_file.fq"}})
    {
        record.sequence();// Amino acid sequence;
    }

    // case 3:
    // I would expect nothing; does not compile
    // auto = sequence_record_native_type all functions like sequence do not compile with static_assert "Please specify record type"
    for (auto && record_native : sequence_file_input{"my_file.fq"}}) // native should be proxy (pr-value) and move-only
    {
        sequence_record<aa27> record{record_native}; // might throw here if parse error if it has copy semantic
        sequence_record_proxy<aa27> record_proxy{record_native}; // lazy evaluation for micro-optimisation?

        // further considerations
        fasta_record record_fasta{record_native}; // does not compile; it's arguable since it has the full informations
        genbank_record record_genbank{record_native}; // does not compile; because of information loss

        sequence_record<aa27> record_{genbank_record}; // might work?
    }

    // PREFFERED:
    for (sequence_record<aa27> record : sequence_file_input{"my_file.fq"}}) // sequence_file_input is not defined over record_type
    {
        record.sequence();// Amino acid sequence;
    }

    // ------------------------------------------------------------------------
    // truncate_id
    // ------------------------------------------------------------------------
    // Over engineering for something that can be done so easily by the user.
    // Better just have dedicated recipe for this using a view.

    constexpr auto truncate_id = [] (std::string id) { return id.substr(id.find(' ')); };
    for (sequence_record && record : sequence_file_input{"my_file.[fq,fa]"}})
    {
        truncate_id(record.id); // maybe as an alternative.
        record.truncated_id(); // dedicated function of the sequence record.
        // User can always do:
        record.id() | std::views::take_while(!seqan3::is_space);
    }
}

// ------------------------------------------------------------------------
// Use Case 3: Read a sam file or bam file: Including the reference sequences, filter by mapping quality, and output the alignment.
// ------------------------------------------------------------------------

#include <seqan3/io/sam_file/all.hpp>
#include <seqan3/core/debug_stream.hpp>

using namespace seqan3;

int main()
{
    std::unordered_map<std::string_view, std::span<seqan3::dna4>> id_sequence_map{};

    std::vector<std::string> ids{};
    std::vector<seqan3::dna5_vector> sequences{};

    // step1: read ids and sequences from fasta file
    // step2: fill id_sequence_map

    // use case1:
    for (seqan3::sam_record<seqan3::dna5, seqan3::phred42> record : sam_file_input{"my_file.bam"})
    {
        if (record.mapping_quality() > 10) // bad quality
            continue;

        seqan3::debug_stream << convert_to_alignment(record, id_sequence_map) << std::endl;
        record.alignment(id_sequence_map); // is this better ?
        // how to persist an alignment?
    }

    // use case1.5: how to write header / footer
    sam_file_input sam_file { "my_file.bam" };
    sam_header header = sam_file.header(); // parses header
    sam_file_input in{};
    in | std::filter() | sam_file_output{in.header(), in.footer()};
    sam_file_output out;
    out.push_back(alignment_record{header});
    // header from record :)

    for (seqan3::sam_record<seqan3::dna5, seqan3::phred42> record : sam_file) // file.records()
    {
        if (record.mapping_quality() > 10) // bad quality
            continue;

        seqan3::debug_stream << convert_to_alignment(record, id_sequence_map) << std::endl;
        record.alignment(id_sequence_map); // is this better ?
        // how to persist an alignment?
    }

    // Use case - pure alignment file:
    for (alignment_native_record native_record : alignment_file_input{"my_file.bam", id_sequence_map})
    {
        alignment_record<seqan3::dna5 /*alignment::sequence::value*/> record{native_record, id_sequence_map};  // the sequence type
        // have explicit functions for different alignment builder
        try{
            record.alignment();
        }
        catch (...)
        {
            try{
                record.alignment_by_map(id_sequence_map);
            }
            catch(...)
            {
                throw "Oh oh!\n";
            }
        }
        // have a single function for the alignment builder
        // if deep copy record derived by template argument of record
        //   return type std::pair<seqan3::gap_decorator<std::span<seqan3::dna5>>, same_as 1st>
        // otherwise if proxy record
        //   return type std::pair<seqan3::gap_decorator<SOME_CRAZY_VIEW>, same_as 1st>
        record.alignment(); // can I use this without map? rrahn: yes, but it throws if the format requires a map.
        // if deep copy record derived by template argument of record
        //   return type std::pair<seqan3::gap_decorator<std::views::all_t<typename id_sequence_map_t::value_type>>,
        //                         seqan3::gap_decorator<std::span<seqan3::dna5>>>
        record.alignment(id_sequence_map); // id_sequence_map is maybe unused in the builder

        // what is with multiple alignments?
        record.alignment(id_sequence_map);
        // return type std::vector<seqan3::gap_decorator<SOME_CRAZY_VIEW>>
    }

    // use case3: if we give file the sequence-map, it needs to pass it to the record
    alignment_file_input alignment_file{"my_file.bam"};
    for (seqan3::alignment_record record : alignment_file)
    {
        record.id_sequence_map(id_sequence_map); // that means you could also set manually
        record.alignment(); // only to allow this
        record.alignment(id_sequence_map); // rrahn & marehr: I like this better
        // What happens with self-contained formats that do not need the additional information of the map?

    }

    // use case4: pass id-map to class
    alignment_file_input alignment_file{"my_file.bam", id_sequence_map};
    alignment_file.id_sequence_map(id_sequence_map); // or is not minimal design

    for (seqan3::alignment_record record : alignment_file)
    {
        record.alignment();
    }
}

// Read a sam file or bam file: Including the reference sequences, filter by mapping quality, and output the alignment.

// How would an index file (.fai) be passed / used?
// How to do random access?
// Filter sam file based on mapping quality and pass everything as is to the output file?
