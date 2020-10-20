#pragma once

#include <seqan3/io/stream/iterator.hpp>

namespace seqan3::awesome
{

template <typename record_base_t>
class format_base
{
protected:

    using streambuf_iterator = seqan3::detail::fast_istreambuf_iterator<char>;

    // template <typename delimiter_t>
    // size_t read_until(streambuf_iterator & it, sequence_record & record, delimiter_t && delimiter)
    // {
    //     // Now we want to actually parse the record.
    //     // We actually run over the stream and delimit the field.
    //     // We might be able to also provide some skip characters? Or we can do this later.
    //     // When we actually apply the conversion.

    //     size_t current_data_size = record.buffer.size();

    //     // We risk elementwise copy here, but want probably some optimisations, like memcpy.
    //     // So we could scan until the delimter and read in the chunk.
    //     // std::ranges::copy_if(it, streambuf_iterator{}, std::cpp20::back_inserter(record.buffer), !delimiter);
    //     for (; it != std::default_sentinel && !delimiter(*it); ++it)
    //         record.buffer.push_back(*it);

    //     return record.buffer.size() - current_data_size;
    // }

    // template <typename delimiter_t>
    // void skip_until(streambuf_iterator & streambuf_it,
    //                 delimiter_t && delimiter)
    // {
    //     for (; streambuf_it != std::default_sentinel && !delimiter(*streambuf_it); ++streambuf_it)
    //     {}
    // }

    // void set_buffer_position(sequence_record & record,
    //                          field const field_id,
    //                          std::pair<size_t, size_t> buffer_pos)
    // {
    //     record.buffer_field_positions[static_cast<int32_t>(field_id)] = std::move(buffer_pos);
    // }

public:

    using record_type = record_base_t;

    format_base() = default;
    format_base(format_base const &) = default;
    format_base(format_base &&) = default;
    format_base & operator=(format_base const &) = default;
    format_base & operator=(format_base &&) = default;
    virtual ~format_base() = default;

    virtual record_base_t & read_record(streambuf_iterator &) = 0;
    // virtual void write_record(streambuf_iterator &, record_base_t &) = 0;
    virtual std::vector<std::string> & extensions() = 0;
};
} // namespace seqan3::awesome
