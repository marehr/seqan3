#pragma once

#include <seqan3/nio/stream/parser_istream.hpp>
#include <seqan3/nio/stream/detail/streambuf_subrange.hpp>

namespace seqan3::nio
{

template <typename char_t, typename traits_t = std::char_traits<char_t>>
struct sequence_file_input_format
{
    struct record_buffer
    {
        using id_t = detail::streambuf_subrange<char_t, traits_t>;
        using sequence_t = detail::streambuf_subrange<char_t, traits_t>;
        using quality_t = detail::streambuf_subrange<char_t, traits_t>;

        id_t id{}; // figure out typical length of id
        sequence_t sequence{}; // figure out typical length of sequence
        quality_t quality{}; // figure out typical length of sequence

        void clear()
        {
            id.clear();
            sequence.clear();
            quality.clear();
        }
    };

    virtual void read_sequence_record(record_buffer &, parser_istream<char_t, traits_t> &);
};

}
