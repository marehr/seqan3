#pragma once

#include <cassert>

#include <seqan3/std/new>
#include <seqan3/io/stream/detail/stream_buffer_exposer.hpp>
#include <seqan3/nio/sequence_file/format/sequence_file_input_format.hpp>
// #include <seqan3/core/char_operations/predicate.hpp>
// #include <seqan3/core/add_enum_bitwise_operators.hpp>


// namespace seqan3::nio
// {
//
// enum struct format_fasta_traits
// {
//     complete_id = 0b0,
//     truncate_id = 0b1,
//
//     parse_alphabet = 0b000, // default for alphabets != char
//     parse_char_alphabet = 0b010, // default for alphabets == char
//     parse_alphabet_non_conforming = 0b100 // for alphabets != char that contain non-conforming characters
// };
//
// } // namespace seqan3::nio
//
// namespace seqan3
// {
// template <>
// constexpr bool add_enum_bitwise_operators<nio::format_fasta_traits> = true;
// } // namespace seqan3

namespace seqan3::nio
{

struct format_fasta_traits
{
    enum id_mode_t
    {
        complete_id,
        truncate_id
    };

    enum sequence_mode_t
    {
        parse_alphabet, // default for alphabets != char
        parse_char_alphabet, // default for alphabets == char
        parse_alphabet_non_conforming // for alphabets != char that contain non-conforming characters
    };
};

template <format_fasta_traits::id_mode_t id_mode, format_fasta_traits::sequence_mode_t sequence_mode>
struct basic_format_fasta : public sequence_file_input_format<char>
{
    using sequence_file_input_format<char>::record_buffer;

    // static constexpr format_fasta_traits id_mode = traits & format_fasta_traits{0b1};
    // static constexpr format_fasta_traits sequence_mode = traits & format_fasta_traits{0b110};

    // Note: alignas guarantees that function begins on cache line
    // Important: This class is not allowed to have a state, i.e. have member variables, otherwise this class will
    // be miss-aligned and cause serious performance degradation.
    virtual void read_sequence_record alignas(std::hardware_destructive_interference_size)
        (record_buffer & record, parser_istream<char> & parser) override
    {
        // ID
        read_id(record.id, parser);

        // Sequence
        read_sequence(record.sequence, parser);
    }

private:
    static constexpr auto const is_blank = [](char const chr)
    {
        bool success = chr == '\t' || chr == ' ';
        // std::cout << "is_blank(chr): " << chr << " :: " << success << std::endl;
        return success;
    };

    static constexpr auto const is_space = [](char const chr)
    {
        bool success = ('\t' <= chr && chr <= '\r') || chr == ' ';
        // std::cout << "is_space(chr): " << chr << " :: " << success << std::endl;
        return success;
    };

    static constexpr auto const is_newline = [](char const chr)
    {
        bool success = chr == '\n';
        // std::cout << "is_space(chr): " << chr << " :: " << success << std::endl;
        return success;
    };

    static constexpr auto const is_digit = [](char const chr)
    {
        bool success = ('0' <= chr && chr <= '9');
        // std::cout << "is_digit(chr): " << chr << " :: " << success << std::endl;
        return success;
    };

    static constexpr auto const is_cntrl = [](char const chr)
    {
        bool success = ('\0' <= chr && chr <= static_cast<char>(31)) || (static_cast<char>(127) == chr);
        // std::cout << "is_cntrl(chr): " << chr << " :: " << success << std::endl;
        return success;
    };

    static constexpr auto const fasta_id_start_token = [](char const chr)
    {
        bool success;
        if constexpr (sequence_mode == format_fasta_traits::parse_alphabet_non_conforming)
            success = chr == '>' || chr == ';';
        else
            success = chr == '>';
        // std::cout << "fasta_id_start_token(chr): " << chr << " :: " << success << std::endl;
        return success;
    };

    static constexpr auto const fasta_truncate_id_token = [](char const chr)
    {
        bool success = is_cntrl(chr) || is_blank(chr);
        // std::cout << "fasta_id_start_token(chr): " << chr << " :: " << success << std::endl;
        return success;
    };

    static void read_id(typename record_buffer::id_t & id, parser_istream<char> & parser)
    {
        if (!parser.advance_if(fasta_id_start_token))
            throw std::runtime_error{"ERROR: expected id character"};

        // skip blanks
        // std::cout << "skip blanks" << std::endl;
        parser.drop_while(is_blank);
        // std::cout << std::endl;

        // bool at_delimiter = false;
        // std::cout << "capture id" << std::endl;
        // until first blank or first cntrl character
        if constexpr (id_mode == format_fasta_traits::truncate_id)
        {
            parser.take_until(id, fasta_truncate_id_token);
            parser.drop_until(is_newline);
        }
        else
        {
            parser.take_until(id, is_newline);
        }

        if (parser.at_eof())
            throw std::runtime_error{"FastA ID line did not end in newline."};

    }

    static void read_sequence(typename record_buffer::sequence_t & sequence, parser_istream<char> & parser)
    {
        // TODO: if char is read; ignore only newlines
        auto const ignore_whitespaces = [](char chr)
        {
            if constexpr (sequence_mode == format_fasta_traits::parse_char_alphabet)
                return is_newline(chr);
            else if constexpr (sequence_mode == format_fasta_traits::parse_alphabet)
                return is_space(chr);
            else if constexpr (sequence_mode == format_fasta_traits::parse_alphabet_non_conforming)
                return is_space(chr) || is_digit(chr);
        };

        parser.take_until_but_ignore(sequence, fasta_id_start_token, ignore_whitespaces);
    }
};

using format_fasta = basic_format_fasta<format_fasta_traits::complete_id, format_fasta_traits::parse_alphabet>;

// assert that format_fasta does not declare any member variable
static_assert(sizeof(format_fasta) <= sizeof(sequence_file_input_format<char>));

} // namespace seqan3
