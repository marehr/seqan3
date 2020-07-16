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
        read_sequence<true>(record.sequence, parser);
    }

    static constexpr auto const is_id = [](char const chr)
    {
        bool success = chr == '>' || chr == ';';
        // std::cout << "is_id(chr): " << chr << " :: " << success << std::endl;
        return success;
    };

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

    template <bool truncate_ids>
    void read_id(typename record_buffer::id_t & id, parser_istream<char> & parser)
    {
        if (!parser.advance_if(is_id))
            throw std::runtime_error{"ERROR: expected id character"};

        // skip blanks
        // std::cout << "skip blanks" << std::endl;
        parser.drop_while(is_blank);
        // std::cout << std::endl;

        // bool at_delimiter = false;
        // std::cout << "capture id" << std::endl;
        // until first blank or first cntrl character
        parser.take_until(id, [](char chr)
        {
            if constexpr (truncate_ids)
                return is_cntrl(chr) || is_blank(chr);
            else
                return chr == '\n';
        });

        if constexpr(truncate_ids)
            parser.drop_while([](char chr)
            {
                return chr != '\n';
            });

        if (parser.at_eof())
            throw std::runtime_error{"FastA ID line did not end in newline."};

    }

    template <bool seqan2_parsing>
    void read_sequence(typename record_buffer::sequence_t & sequence, parser_istream<char> & parser)
    {
        while (true)
        {
            // skip space or digit character
            // std::cout << "skip spaces / digits" << std::endl;
            parser.drop_while([](char chr)
            {
                if constexpr (seqan2_parsing)
                    return is_space(chr) /*|| is_digit(chr)*/;
                else
                    return is_space(chr) || is_digit(chr);
            });

            // stop reading the sequence if we found the start character of the next id, i.e. ">"
            if (parser.at_eof() || parser.char_is(is_id))
                return;

            // read (possible empty) sequence (until the first id, space or digit)
            // std::cout << "capture sequence" << std::endl;
            parser.take_until(sequence, [](char chr)
            {
                if constexpr (seqan2_parsing)
                    return is_id(chr) || is_space(chr) /*|| is_digit(chr)*/;
                else
                    return is_id(chr) || is_space(chr) || is_digit(chr);
            });
        }
    }
};

} // namespace seqan3
