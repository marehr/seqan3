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

class read_policy
{
public:
    read_policy() = default;
    read_policy(read_policy const &) = default;
    read_policy(read_policy &&) = default;
    read_policy & operator=(read_policy const &) = default;
    read_policy & operator=(read_policy &&) = default;
    ~read_policy() = default;

protected:

    template <typename buffer_t, typename delimiter_t>
    bool read_until(buffer_t & buffer,
                    delimiter_t && delimiter,
                    seqan3::detail::stream_buffer_exposer<char> & streambuf)
    {
        char * current = streambuf.gptr();
        // This assumes we know that we don't need to call underflow.
        for (; current != streambuf.egptr() && !delimiter(*current); ++current)
        {}

        size_t old_buffer_size = buffer.size();
        size_t count = current - streambuf.gptr();
        buffer.resize(old_buffer_size + count); // make enough memory space. // Use pmr::vector for this.
        std::memcpy(buffer.data() + old_buffer_size, streambuf.gptr(), count);
        streambuf.gbump(count);

        // We actually found the delimiter and do not need to underflow.
        if (delimiter(*current))
            return true;
        else if (seqan3::is_eof(streambuf.underflow()))
            return false;

        return read_until(buffer, delimiter, streambuf);
    }
};

class skip_policy
{
public:
    skip_policy() = default;
    skip_policy(skip_policy const &) = default;
    skip_policy(skip_policy &&) = default;
    skip_policy & operator=(skip_policy const &) = default;
    skip_policy & operator=(skip_policy &&) = default;
    ~skip_policy() = default;

protected:

    template <typename delimiter_t>
    bool skip_until(delimiter_t && delimiter, seqan3::detail::stream_buffer_exposer<char> & streambuf)
    {
        char * current = streambuf.gptr();
        // This assumes we know that we don't need to call underflow.
        for (; current != streambuf.egptr() && !delimiter(*current); ++current)
        {}

        streambuf.gbump(current - streambuf.gptr());

        // We actually found the delimiter and do not need to underflow.
        if (delimiter(*current))
            return true;
        else if (seqan3::is_eof(streambuf.underflow())) // we need to underflow and check if we are at the end.
            return false;

        return skip_until(delimiter, streambuf); // we skip more.
    }
};
} // namespace seqan3::awesome
