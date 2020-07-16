#pragma once

#include <cassert>

#include <seqan3/io/stream/detail/stream_buffer_exposer.hpp>
#include <seqan3/nio/sequence_file/format/sequence_file_input_format.hpp>
// #include <seqan3/core/char_operations/predicate.hpp>

namespace seqan3::nio
{

struct format_fasta : public sequence_file_input_format<char>
{
    using sequence_file_input_format<char>::record_buffer;

    virtual void read_sequence_record(record_buffer & record, parser_istream<char> & parser) override
    {
        // std::cout << "read_sequence_record: record" << std::endl;
        // ID
        read_id<false>(record.id, parser);

        // Sequence
        read_sequence<true, false>(record.sequence, parser);
    }

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
        bool success = chr == '>' || chr == ';';
        // std::cout << "fasta_id_start_token(chr): " << chr << " :: " << success << std::endl;
        return success;
    };

    static constexpr auto const fasta_truncate_id_token = [](char const chr)
    {
        bool success = is_cntrl(chr) || is_blank(chr);
        // std::cout << "fasta_id_start_token(chr): " << chr << " :: " << success << std::endl;
        return success;
    };

    template <bool truncate_ids>
    void read_id(typename record_buffer::id_t & id, parser_istream<char> & parser)
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
        if constexpr (truncate_ids)
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

    template <bool seqan2_parsing, bool parse_alphabet>
    void read_sequence(typename record_buffer::sequence_t & sequence, parser_istream<char> & parser)
    {
        // TODO: if char is read; ignore only newlines
        auto const ignore_whitespaces = [](char chr)
        {
            if constexpr (!parse_alphabet)
                return is_newline(chr);
            else if constexpr (seqan2_parsing)
                return is_space(chr);
            else
                return is_space(chr) || is_digit(chr);
        };

        parser.take_until_but_ignore(sequence, fasta_id_start_token, ignore_whitespaces);
    }
};

} // namespace seqan3
