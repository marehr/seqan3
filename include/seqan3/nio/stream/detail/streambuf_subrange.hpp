#pragma once

#include <cassert>
#include <seqan3/std/span>
#include <vector>

namespace seqan3::nio::detail
{

// IDEA: benutze span vom unterliegendem stream, oder kopiert den span in den vector
template <typename char_t, typename traits_t = std::char_traits<char_t>>
struct streambuf_subrange
{
    streambuf_subrange() = default;
    explicit streambuf_subrange(size_t capacity)
        : streambuf_copy{}
    {
        streambuf_copy.reserve(capacity);
    }

    struct segmented_span : protected std::vector<std::span<char_t>>
    {
        using base_t = std::vector<std::span<char_t>>;

        using base_t::emplace_back;
        using base_t::clear;
        using base_t::begin;
        using base_t::end;
        using base_t::back;
    };

    // using container_t = std::string;//std::vector<char_t>;
    using container_t = std::vector<char_t>;

    operator std::string () &&
    {
        std::string str{};
        append_to(str);
        clear();
        return str;
    }

    operator /*std::vector<char_t>*/ container_t () &&
    {
        flush();
        container_t str{std::move(streambuf_copy)};
        streambuf_copy.reserve(str.capacity());
        return str;
    }

    void segment_start(char_t * start)
    {
        // std::cout << "segment_start: " << reinterpret_cast<size_t>(start) << std::endl;
        assert(start != nullptr);
        streambuf_segment_start = start;
    }

    void segment_end(char_t * end)
    {
        // std::cout << "segment_end: " << reinterpret_cast<size_t>(end) << std::endl;
        assert(end != nullptr);
        if (streambuf_segment_start)
            streambuf_segmented_span.emplace_back(streambuf_segment_start, end);
        // for (auto && span : segments())
        // {
        //     // std::cout << "captured: " << std::string(span.begin(), span.end()) << std::endl;
        // }
        streambuf_segment_start = nullptr;
    }

    segmented_span & segments()
    {
        // as long as we don't have any data copied return shallow reference into the buffer
        if (streambuf_copy.size() == 0u)
            return streambuf_segmented_span;

        // if we already have a copy, flush the last segments into it and return
        flush();
        streambuf_segmented_copy.clear();
        streambuf_segmented_copy.emplace_back(streambuf_copy.begin(), streambuf_copy.end());
        return streambuf_segmented_copy;
    }

    template <typename container_t>
    void append_to(container_t & container) const
    {
        container.insert(container.end(), streambuf_copy.begin(), streambuf_copy.end());
        pending_append_to(container);
    }

    void flush()
    {
        // std::cout << "streambuf_subrange::flush" << std::endl;
        assert(streambuf_segment_start == nullptr);
        pending_append_to(streambuf_copy);
        streambuf_segmented_span.clear();
    }

    void clear()
    {
        streambuf_copy.clear();
        streambuf_segmented_span.clear();
    }

private:
    template <typename container_t>
    void pending_append_to(container_t & container) const
    {
        for (auto && span : streambuf_segmented_span)
            container.insert(container.end(), span.begin(), span.end());
    }

    char_t* streambuf_segment_start{nullptr};
    segmented_span streambuf_segmented_span{};

    container_t streambuf_copy{};
    segmented_span streambuf_segmented_copy{};
};

} // namespace seqan3::detail
