#pragma once

#include <seqan3/io/stream/iterator.hpp>

namespace seqan3::awesome
{

template <typename record_base_t>
class format_base
{
protected:

    using istream_type = std::basic_istream<char>;

public:

    using record_type = record_base_t;

    format_base() = default;
    format_base(format_base const &) = default;
    format_base(format_base &&) = default;
    format_base & operator=(format_base const &) = default;
    format_base & operator=(format_base &&) = default;
    virtual ~format_base() = default;

    virtual record_base_t * read_record(istream_type &) = 0;
    // virtual void write_record(streambuf_iterator &, record_base_t &) = 0;
    virtual std::vector<std::string> & extensions() = 0;
};
} // namespace seqan3::awesome
