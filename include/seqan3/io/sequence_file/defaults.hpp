
#pragma once

#include <seqan3/alphabet/adaptation/char.hpp>
#include <seqan3/alphabet/aminoacid/aa27.hpp>
#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/alphabet/nucleotide/dna15.hpp>
#include <seqan3/alphabet/quality/phred42.hpp>
#include <seqan3/alphabet/quality/qualified.hpp>

#include <seqan3/io/sequence_file/format_embl.hpp>
#include <seqan3/io/sequence_file/format_fasta.hpp>
#include <seqan3/io/sequence_file/format_fastq.hpp>
#include <seqan3/io/sequence_file/format_genbank.hpp>
#include <seqan3/io/alignment_file/format_sam.hpp>

#include <seqan3/io/detail/record.hpp>
#include <seqan3/io/sequence_file/input_format_concept.hpp>

namespace seqan3
{

// forward
template <
    typename /*sequence_file_input_traits*/ traits_type_ = sequence_file_input_defaults::traits_type,
    detail::fields_specialisation selected_field_ids_ = sequence_file_input_defaults::selected_field_ids,
    detail::type_list_of_sequence_file_input_formats valid_formats_ = sequence_file_input_defaults::formats>
class sequence_file_input;

// ----------------------------------------------------------------------------
// sequence_file_input_default_traits
// ----------------------------------------------------------------------------

/*!\brief The default traits for seqan3::sequence_file_input
 * \implements sequence_file_input_traits
 * \ingroup sequence
 *
 * \details
 *
 * If you wish to change a single or a few types from the default, just inherit from this class and
 * "overwrite" the respective type definitions.
 *
 * This example will make the file read into a smaller alphabet and a compressed container:
 *
 * \include test/snippet/io/sequence_file/sequence_file_input_trait_overwrite.cpp
 */
struct sequence_file_input_default_traits_dna
{
    /*!\name Member types
     * \brief Definitions to satisfy seqan3::sequence_file_input_traits.
     * \{
     */

    //!\brief The sequence alphabet is seqan3::dna5.
    using sequence_alphabet                 = dna5;

    //!\brief The legal sequence alphabet for parsing is seqan3::dna15.
    using sequence_legal_alphabet           = dna15;

    //!\brief The type of a DNA sequence is std::vector.
    // template <typename _sequence_alphabet>
    using sequence/*_container*/                = std::vector<sequence_alphabet>;

    //!\brief The alphabet for an identifier string is char.
    using id_alphabet                       = char;

    //!\brief The string type for an identifier is std::basic_string.
    // template <typename _id_alphabet>
    using id/*_container*/                      = std::basic_string<id_alphabet>;

    //!\brief The alphabet for a quality annotation is seqan3::phred42.
    using quality_alphabet                  = phred42;

    //!\brief The string type for a quality annotation is std::vector.
    // template <typename _quality_alphabet>
    using quality/*_container*/                 = std::vector<phred42>;

    //!\}
};

//!\brief A traits type that specifies input as amino acids.
//!\ingroup sequence
struct sequence_file_input_default_traits_aa : sequence_file_input_default_traits_dna
{
    /*!\name Member types
     * \brief Definitions to satisfy seqan3::sequence_file_input_traits.
     * \{
     */

    //!\brief The sequence alphabet is seqan3::aa27.
    using sequence_alphabet = aa27;

    //!\brief The legal sequence alphabet for parsing is seqan3::aa27.
    using sequence_legal_alphabet = aa27;

    using sequence/*_container*/ = std::vector<aa27>;
    //!\}
};

/*!\name Type deduction guides
 * \relates seqan3::sequence_file_input
 * \{
 */

struct sequence_file_input_defaults
{
    using traits_type = sequence_file_input_default_traits_dna;
    using selected_field_ids = fields<field::seq, field::id, field::qual>;
    using formats = type_list<format_embl, format_fasta, format_fastq, format_genbank, format_sam>;
};

//!\brief Deduces the sequence input file type from the stream and the format.

template <typename selected_field_ids = sequence_file_input_defaults::selected_field_ids>
sequence_file_input(std::filesystem::path filename,
                    selected_field_ids const & = sequence_file_input_defaults::selected_field_ids{})
    -> sequence_file_input<sequence_file_input_defaults::traits_type,         // actually use the default
                           selected_field_ids,  // default field ids.
                           sequence_file_input_defaults::formats>;

template <input_stream stream_type,
          sequence_file_input_format file_format>
sequence_file_input(stream_type & stream,
                    file_format const &)
    -> sequence_file_input<sequence_file_input_defaults::traits_type,         // actually use the default
                           sequence_file_input_defaults::selected_field_ids,  // default field ids.
                           type_list<file_format>>;

//!\overload
template <input_stream stream_type,
          sequence_file_input_format file_format>
sequence_file_input(stream_type && stream,
                    file_format const &)
   -> sequence_file_input<sequence_file_input_defaults::traits_type,         // actually use the default
                          sequence_file_input_defaults::selected_field_ids,  // default field ids.
                          type_list<file_format>>;

//!\brief Deduces the sequence input file type from the stream, the format and the field ids.
template <input_stream stream_type,
          sequence_file_input_format file_format,
          detail::fields_specialisation selected_field_ids>
sequence_file_input(stream_type && stream,
                    file_format const &,
                    selected_field_ids const &)
    -> sequence_file_input<sequence_file_input_defaults::traits_type,       // actually use the default
                           selected_field_ids,
                           type_list<file_format>>;

//!\overload
template <input_stream stream_type,
          sequence_file_input_format file_format,
          detail::fields_specialisation selected_field_ids>
sequence_file_input(stream_type & stream,
                    file_format const &,
                    selected_field_ids const &)
    -> sequence_file_input<sequence_file_input_defaults::traits_type,       // actually use the default
                           selected_field_ids,
                           type_list<file_format>>;
//!\}

} // namespace seqan3
