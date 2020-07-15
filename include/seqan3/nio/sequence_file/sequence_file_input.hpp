#pragma once

#include <functional>
#include <memory>

#include <seqan3/nio/sequence_file/format/sequence_file_input_format.hpp>
#include <seqan3/nio/stream/parser_istream.hpp>

namespace seqan3::nio
{

template <typename = void>
class sequence_file_input
{
protected:
    using stream_char_type = char;
    using stream_traits_type = std::char_traits<stream_char_type>;
public:
    sequence_file_input() = delete;
    sequence_file_input(sequence_file_input const &) = delete;
    sequence_file_input & operator=(sequence_file_input const &) = delete;
    sequence_file_input(sequence_file_input &&) = default;
    sequence_file_input & operator=(sequence_file_input &&) = default;
    ~sequence_file_input() = default;

    // template <typename stream_t>
    // // //!\cond
    // //     requires std::same_as<typename std::remove_reference_t<stream_t>::char_type, stream_char_type>
    // // //!\endcond
    sequence_file_input(std::basic_istream<stream_char_type, stream_traits_type> & stream) :
        primary_stream{&stream, stream_deleter_noop},
        // format{detail::sequence_file_input_format_exposer<file_format>{}}
        // parser{*stream.rdbuf()}
        parser{*primary_stream->rdbuf()}
    {
        // static_assert(list_traits::contains<file_format, valid_formats>,
        //               "You selected a format that is not in the valid_formats of this file.");

        // possibly add intermediate compression stream
        // secondary_stream = detail::make_secondary_istream(*primary_stream);
    }

    struct record;
    struct iterator;
    using sentinel = std::default_sentinel_t;

    void set_format(sequence_file_input_format<char> & format)
    {
        this->format = &format;
    }

    iterator begin()
    {
        if (format == nullptr)
            throw std::runtime_error{"sequence_file_input: format was not set."};

        assert(format != nullptr);
        if (!first_record_was_read)
        {
            read_next_record();
            first_record_was_read = true;
        }
        return {*this};
    }

    sentinel end()
    {
        return {};
    }

    // reference front() noexcept
    // {
    //     return *begin();
    // }
    //!\}

    // //!\brief The options are public and its members can be set directly.
    // sequence_file_input_options<typename traits_type::sequence_legal_alphabet,
    //                          selected_field_ids::contains(field::seq_qual)> options;

protected:
    using parser_t = parser_istream<stream_char_type, stream_traits_type>;
    using format_t = sequence_file_input_format<stream_char_type, stream_traits_type>;

    //!\privatesection
    /*!\name Data buffers
     * \{
     */
    //!\brief Buffer for a single record.
    format_t::record_buffer record_buffer{};
    //!\brief A larger (compared to stl default) stream buffer to use when reading from a file.
    // std::vector<char> stream_buffer{std::vector<char>(1'000'000)};
    //!\}

    /*!\name Stream / file access
     * \{
     */
    //!\brief The type of the internal stream pointers. Allows dynamically setting ownership management.
    using stream_ptr_t = std::unique_ptr<std::basic_istream<stream_char_type>,
                                         std::function<void(std::basic_istream<stream_char_type>*)>>;
    // // //!\brief Stream deleter that does nothing (no ownership assumed).
    static void stream_deleter_noop(std::basic_istream<stream_char_type> *) {}
    //!\brief Stream deleter with default behaviour (ownership assumed).
    static void stream_deleter_default(std::basic_istream<stream_char_type> * ptr) { delete ptr; }

    //!\brief The primary stream is the user provided stream or the file stream if constructed from filename.
    stream_ptr_t primary_stream{nullptr, stream_deleter_noop};

    // //!\brief The secondary stream is a compression layer on the primary or just points to the primary (no compression).
    // stream_ptr_t secondary_stream{nullptr, stream_deleter_noop};

    parser_t parser;

    //!\brief Tracks whether the very first record is buffered when calling begin().
    bool first_record_was_read{false};
    //!\brief File is at position 1 behind the last record.
    bool at_end{false};

    // //!\brief Type of the format, an std::variant over the `valid_formats`.
    // using format_type = typename detail::variant_from_tags<valid_formats,
    //                                                        detail::sequence_file_input_format_exposer>::type;
    //!\brief The actual std::variant holding a pointer to the detected/selected format.
    format_t * format{nullptr};
    //!\}

    //!\brief Tell the format to move to the next record and update the buffer.
    void read_next_record()
    {
        // clear the record
        record_buffer.clear();

        // at end if we could not read further
        at_end = parser.at_eof();
        if (at_end)
            return;

        // read new record
        // TODO: support don't reading in stuff
        assert(format != nullptr);
        format->read_sequence_record(record_buffer, parser);
    }

    // //!\brief Befriend iterator so it can access the buffers.
    // friend iterator;
};

template <typename __TODO__>
struct sequence_file_input<__TODO__>::iterator
{
    using base_t = seqan3::detail::fast_istreambuf_iterator<stream_char_type, stream_traits_type>;
    record & operator*()
    {
        if (_record_empty)
        {
            _record = record
            {
                std::move(parent.record_buffer.id),
                std::move(parent.record_buffer.sequence),
                std::move(parent.record_buffer.quality)
            };
            _record_empty = false;
        }
        return _record;
    }

    void operator++()
    {
        _record_empty = true;
        parent.read_next_record();
    }

    bool operator==(sentinel) const
    {
        return parent.at_end;
    }

    bool operator!=(sentinel sentinel) const
    {
        return !(*this == sentinel);
    }

    sequence_file_input & parent;
    record _record{};
    bool _record_empty = true;
};

template <typename __TODO__>
struct sequence_file_input<__TODO__>::record
{
    std::string & id()
    {
        return _id;
    }

    std::string & sequence()
    {
        return _sequence;
    }

    std::string & quality()
    {
        return _quality;
    }

    std::string _id{};
    std::string _sequence{};
    std::string _quality{};
};

// /*!\name Type deduction guides
//  * \relates seqan3::sequence_file_input
//  * \{
//  */
//
// //!\brief Deduces the sequence input file type from the stream and the format.
// template <input_stream stream_type,
//           sequence_file_input_format file_format>
// sequence_file_input(stream_type & stream,
//                     file_format const &)
//     -> sequence_file_input<typename sequence_file_input<>::traits_type,         // actually use the default
//                            typename sequence_file_input<>::selected_field_ids,  // default field ids.
//                            type_list<file_format>>;
//
// //!\overload
// template <input_stream stream_type,
//           sequence_file_input_format file_format>
// sequence_file_input(stream_type && stream,
//                     file_format const &)
//    -> sequence_file_input<typename sequence_file_input<>::traits_type,         // actually use the default
//                           typename sequence_file_input<>::selected_field_ids,  // default field ids.
//                           type_list<file_format>>;
//
// //!\brief Deduces the sequence input file type from the stream, the format and the field ids.
// template <input_stream stream_type,
//           sequence_file_input_format file_format,
//           detail::fields_specialisation selected_field_ids>
// sequence_file_input(stream_type && stream,
//                     file_format const &,
//                     selected_field_ids const &)
//     -> sequence_file_input<typename sequence_file_input<>::traits_type,       // actually use the default
//                            selected_field_ids,
//                            type_list<file_format>>;
//
// //!\overload
// template <input_stream stream_type,
//           sequence_file_input_format file_format,
//           detail::fields_specialisation selected_field_ids>
// sequence_file_input(stream_type & stream,
//                     file_format const &,
//                     selected_field_ids const &)
//     -> sequence_file_input<typename sequence_file_input<>::traits_type,       // actually use the default
//                            selected_field_ids,
//                            type_list<file_format>>;
// //!\}

} // namespace seqan3::nio
