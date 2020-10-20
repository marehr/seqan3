#pragma once

#include <seqan3/std/concepts>
#include <seqan3/std/ranges>
#include <string>

#include <seqan3/core/char_operations/predicate.hpp>
#include <seqan3/io/awesome_file/format_base.hpp>
#include <seqan3/io/awesome_file/sequence_record.hpp>
#include <seqan3/range/views/slice.hpp>

#include <seqan3/core/debug_stream.hpp>

namespace seqan3::awesome
{

// The original value. -> store pointer to base type.
// TODO: Refactor into policies for data management.
template <typename base_record_t>
    requires std::derived_from<base_record_t, sequence_record>
class fasta_record final : public base_record_t
{
private:

    using typename base_record_t::return_t;
public:

    using buffer_interval_type = std::pair<size_t, size_t>;

    // // Might have a header pointer.
    std::vector<char> buffer{};
    std::vector<buffer_interval_type> buffer_field_positions{3 , {0, 0}};

    // using sequence_record::sequence_record;

    fasta_record() : base_record_t{this}
    {}

    // TODO: Get rid of this implementation using a) adaptor or b) policy?
    fasta_record(fasta_record const & other) :
        base_record_t{this},
        buffer{other.buffer},
        buffer_field_positions{other.buffer_field_positions}
    {}

    fasta_record(fasta_record && other) noexcept : fasta_record{}
    {
        swap(other);
    }

    fasta_record & operator=(fasta_record other) noexcept
    {
        swap(other);
        return *this;
    }
    ~fasta_record() = default;

    void clear() override
    {
        buffer.clear();
        std::ranges::fill(buffer_field_positions, buffer_interval_type{0, 0});
    }

protected:

    return_t id_impl() override
    {
        int32_t field_pos = static_cast<int32_t>(field::id);
        return buffer | seqan3::views::slice(buffer_field_positions[field_pos].first,
                                             buffer_field_positions[field_pos].second);
    }

    return_t seq_impl() override
    {
        int32_t field_pos = static_cast<int32_t>(field::seq);
        return buffer | seqan3::views::slice(buffer_field_positions[field_pos].first,
                                             buffer_field_positions[field_pos].second);
    }

    void swap(fasta_record & rhs) noexcept
    {
        using std::swap;

        swap(buffer, rhs.buffer);
        swap(buffer_field_positions, rhs.buffer_field_positions);
    }
};

// TODO: Abstract into policies.
// TODO: Optimise the parsing.
template <typename record_base_t = sequence_record>
    requires std::semiregular<fasta_record<record_base_t>>
class format_fasta : public format_base<record_base_t>
{
private:

    using base_format_type = format_base<record_base_t>;
    using record_type = fasta_record<record_base_t>;
    using typename base_format_type::streambuf_iterator;

    static constexpr auto is_id_delimiter = seqan3::is_char<'>'>;

    std::vector<std::string> valid_extensions{{"fa"}, {"fasta"}, {"fn"}};
    record_type record{};

    template <typename>
    friend class format_fasta;

public:
    //!\brief Rebinds this fasta format to a new record base type, i.e. users can extend the seqan3::awesome::sequence_record.
    template <typename new_record_base_t>
    using rebind_record = format_fasta<new_record_base_t>;

    format_fasta() = default;
    format_fasta(format_fasta const &) = default;
    format_fasta(format_fasta &&) = default;
    format_fasta & operator=(format_fasta const &) = default;
    format_fasta & operator=(format_fasta &&) = default;
    ~format_fasta() override = default;

    template <typename other_sequence_record_t>
    explicit format_fasta(format_fasta<other_sequence_record_t> other) :
        valid_extensions{std::move(other.valid_extensions)}
    {
        debug_stream << "format_fast: Calling converting constructor!\n";
    }

    std::vector<std::string> & extensions() override
    {
        return valid_extensions;
    }

    record_type & read_record(streambuf_iterator & it) override
    {
        // for (size_t i = 0; i < 341 && it != std::default_sentinel; ++i)
        // {
        record.clear();
        // Also just parse repeatedly, until delimiter is reached and adding to the buffer
        // And afterwards set the id.
        assert(is_id_delimiter(*it));

        size_t buffer_position = 0;
        ++it;
        skip_until(it, seqan3::is_graph); // Until the first non space character.

        buffer_position += read_until(it, record, seqan3::is_char<'\n'> || seqan3::is_char<'\r'>);
        set_buffer_position(record, field::id, {0, buffer_position});

        auto seq_field_delimiter = is_id_delimiter || seqan3::is_eof;
        size_t seq_buffer_begin = buffer_position;

        while (it != std::default_sentinel && !seq_field_delimiter(*it))
        {
            buffer_position += read_until(it, record, seqan3::is_space);
            skip_until(it, seqan3::is_graph);
        }
        set_buffer_position(record, field::seq, {seq_buffer_begin, buffer_position});
        // }
        return record;
    }

private:
    template <typename delimiter_t>
    size_t read_until(streambuf_iterator & it, record_type & record, delimiter_t && delimiter)
    {
        // Now we want to actually parse the record.
        // We actually run over the stream and delimit the field.
        // We might be able to also provide some skip characters? Or we can do this later.
        // When we actually apply the conversion.

        size_t current_data_size = record.buffer.size();

        // We risk elementwise copy here, but want probably some optimisations, like memcpy.
        // So we could scan until the delimter and read in the chunk.
        // std::ranges::copy_if(it, streambuf_iterator{}, std::cpp20::back_inserter(record.buffer), !delimiter);
        for (; it != std::default_sentinel && !delimiter(*it); ++it)
            record.buffer.push_back(*it);

        return record.buffer.size() - current_data_size;
    }

    template <typename delimiter_t>
    void skip_until(streambuf_iterator & streambuf_it, delimiter_t && delimiter)
    {
        for (; streambuf_it != std::default_sentinel && !delimiter(*streambuf_it); ++streambuf_it)
        {}
    }

    void set_buffer_position(record_type & record,
                             field const field_id,
                             std::pair<size_t, size_t> buffer_pos)
    {
        record.buffer_field_positions[static_cast<int32_t>(field_id)] = std::move(buffer_pos);
    }
};

// Default deduction guide.
format_fasta() -> format_fasta<>;

} // namespace seqan3::awesome
