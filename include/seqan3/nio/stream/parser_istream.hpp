#pragma once

#include <iosfwd>
#include <seqan3/io/stream/detail/fast_istreambuf_iterator.hpp>
#include <seqan3/nio/stream/detail/streambuf_subrange.hpp>

namespace seqan3::nio
{
template <typename char_t, typename traits_t = std::char_traits<char_t>>
struct parser_istream
{
    struct istreambuf;

    parser_istream() = delete;
    parser_istream(std::basic_streambuf<char_t, traits_t> & buf)
        : buffer{*this, buf}
    {}

    auto begin()
    {
        return it;
    }

    auto end()
    {
        return std::default_sentinel;
    }

    using streambuf_subrange_t = detail::streambuf_subrange<char_t, traits_t>;
    streambuf_subrange_t * subrange = nullptr;

    void capture_start(streambuf_subrange_t & subrange)
    {
        // std::cout << "capture_start \n" << std::endl;
        this->subrange = &subrange;
        this->subrange->segment_start(buffer.gptr());
    }

    void capture_end()
    {
        // std::cout << "capture_end \n" << std::endl;
        assert(subrange != nullptr);

        if (!at_eof())
            this->subrange->segment_end(buffer.gptr());

        this->subrange = nullptr;
    }

    bool at_eof()
    {
        assert((begin() == end()) == buffer.at_eof());
        return buffer.at_eof();
    }

    template <typename predicate_t>
    bool char_is(predicate_t && predicate)
    {
        return !at_eof() && predicate(*buffer.gptr());
    }

    template <typename predicate_t>
    bool advance_if(predicate_t && predicate)
    {
        auto it = begin();
        bool success = it != end() && predicate(*it);
        if (success)
            ++it;
        return success;
    }

    template <typename predicate_t>
    void drop_while(predicate_t && predicate)
    {
        _skip_while<false>(predicate);
    }

    template <typename predicate_t>
    void take_until(streambuf_subrange_t & subrange, predicate_t && predicate)
    {
        capture_start(subrange);
        _skip_while<true>(predicate);
        capture_end();
    }

    template <bool predicate_is, typename predicate_t>
    void _skip_while(predicate_t && predicate)
    {
        while (!buffer.at_eof())
        {
            char_t * const begin = buffer.gptr();
            char_t * it = begin;
            char_t * const sentinel = buffer.egptr();
            bool predicate_found{false};

            // iterate over chunk
            for (; it != sentinel; ++it)
            {
                char_t chr = *it;
                if (predicate(chr) == predicate_is)
                {
                    predicate_found = true;
                    break;
                }
            }

            // inform buffer how much we read from the chunk
            buffer.gbump(it - begin);

            if (predicate_found)
                break;

            // we are at the end of the current chunk
            assert(buffer.at_eof());

            // acquire next chunk
            buffer.underflow();
        }
    }

    istreambuf buffer;
    seqan3::detail::fast_istreambuf_iterator<char_t, traits_t> it{buffer};
};

template <typename char_t, typename traits_t>
struct parser_istream<char_t, traits_t>::istreambuf : public std::basic_streambuf<char_t, traits_t>
{
    using base_t = std::basic_streambuf<char_t, traits_t>;
    using exposed_base_t = seqan3::detail::stream_buffer_exposer<char_t, traits_t>;

    istreambuf(parser_istream & parent, base_t & buffer_)
        : parent{parent}, buffer{reinterpret_cast<exposed_base_t &>(buffer_)}
    {
        buffer_updates_this();
    }

    virtual ~istreambuf()
    {
        this_updates_buffer();
    }

    using int_type = typename traits_t::int_type;

    using base_t::gptr;
    using base_t::egptr;
    using base_t::gbump;

    virtual int_type underflow() override
    {
        // std::cout << "underflow?! \n" << std::endl;
        if (!at_eof())
            return traits_t::to_int_type(*gptr());

        // std::cout << "underflow real?! \n" << std::endl;
        if (parent.subrange != nullptr)
        {
            assert(gptr() != nullptr);
            // std::cout << "gptr(): " << reinterpret_cast<size_t>(gptr()) << std::endl;
            parent.subrange->segment_end(gptr());
            parent.subrange->flush();
            // std::cout << "gptr(): " << reinterpret_cast<size_t>(gptr()) << std::endl;
        }

        this_updates_buffer();
        int_type c = buffer.underflow();
        buffer_updates_this();

        if (parent.subrange != nullptr && c != traits_t::eof())
            parent.subrange->segment_start(gptr());

        return c;
    };

    bool at_eof()
    {
        return gptr() == egptr();
    }

    void buffer_updates_this()
    {
        this->setg(buffer.eback(), buffer.gptr(), buffer.egptr());
    }

    void this_updates_buffer()
    {
        buffer.setg(this->eback(), this->gptr(), this->egptr());
    }

    parser_istream & parent;
    exposed_base_t & buffer;
};
} // namespace seqan3
