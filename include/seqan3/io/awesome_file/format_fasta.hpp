#pragma once

#include <cstring>
#include <seqan3/std/concepts>
#include <seqan3/std/ranges>
#include <memory_resource>
#include <vector>
#include <string>

#include <seqan3/core/char_operations/predicate.hpp>
#include <seqan3/io/awesome_file/format_base.hpp>
#include <seqan3/io/awesome_file/sequence_record.hpp>
#include <seqan3/range/views/type_reduce.hpp>

#include <seqan3/core/debug_stream.hpp>

namespace seqan3::awesome
{

// Default definition which is overloaded for the record later.
template <typename base_record_t>
class fasta_record
{
    fasta_record() = delete;
};

// Write a parsing policy?

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

// TODO: Abstract into policies.
// TODO: Optimise the parsing.
template <typename record_base_t = sequence_record>
//!\cond
    // requires std::semiregular<fasta_record<record_base_t>>
//!\endcond
class format_fasta :
    public format_base<record_base_t>,
    private read_policy,
    private skip_policy
{
private:

    using base_format_type = format_base<record_base_t>;
    using record_type = fasta_record<record_base_t>;
    using typename base_format_type::istream_type;

    static constexpr auto is_id_delimiter = seqan3::is_char<'>'>;

    std::vector<std::string> valid_extensions{{"fa"}, {"fasta"}, {"fn"}};
    record_type record{};

    template <typename>
    friend class format_fasta;

    using streambuf_iterator = seqan3::detail::fast_istreambuf_iterator<char>;
    using istreambuf_type = seqan3::detail::stream_buffer_exposer<char>;
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
    //!\cond
        requires (!std::same_as<other_sequence_record_t, record_base_t>)
    //!\endcond
    explicit format_fasta(format_fasta<other_sequence_record_t> other) :
        valid_extensions{std::move(other.valid_extensions)}
    {
        debug_stream << "format_fast: Calling converting constructor!\n";
    }

    std::vector<std::string> & extensions() override
    {
        return valid_extensions;
    }

    record_type * read_record(istream_type & istream) override
    {
        assert(istream.rdbuf() != nullptr);

        istreambuf_type & istreambuf = reinterpret_cast<istreambuf_type &>(*istream.rdbuf());
        streambuf_iterator it{istreambuf};
        if (it == std::default_sentinel)
            return nullptr;

        record.clear();

        assert(is_id_delimiter(*istreambuf.gptr()));

        istreambuf.gbump(1);
        if (!skip_until(seqan3::is_graph, istreambuf))
            throw std::runtime_error{"Unexpected end of input"};

        if (!read_until(record.id_buffer, seqan3::is_char<'\n'> || seqan3::is_char<'\r'>, istreambuf))
            throw std::runtime_error{"Unexpected end of input"};

        constexpr auto seq_field_delimiter = is_id_delimiter || seqan3::is_eof;
        while (it != std::default_sentinel && !seq_field_delimiter(*it))
        {
            if (!read_until(record.seq_buffer, seqan3::is_space, istreambuf))
                throw std::runtime_error{"Unexpected end of input"};

            skip_until(seqan3::is_graph, istreambuf);
        }
        return &record;
    }
};

// ----------------------------------------------------------------------------
// fasta_rectod implementation.
// ----------------------------------------------------------------------------

template <typename base_record_t>
//!\cond
    requires std::derived_from<base_record_t, sequence_record>
//!\endcond
class fasta_record<base_record_t> final :
    public base_record_t,
    private record_registration_policy<fasta_record<base_record_t>>
{
private:
    friend record_registration_policy<fasta_record<base_record_t>>;

    template <typename>
    friend class format_fasta;

    using typename base_record_t::return_t;

    // std::array<char, 2000> buffer{}; // a small buffer on the stack
    // std::shared_ptr<std::pmr::monotonic_buffer_resource> pool_ptr = std::make_shared<std::pmr::monotonic_buffer_resource>(buffer.data(), buffer.size());

    std::vector<char> id_buffer{};
    // std::pmr::vector<char> seq_buffer{pool_ptr.get()};
    std::vector<char> seq_buffer{};
public:

    using buffer_interval_type = std::pair<size_t, size_t>;

    fasta_record() = default;
    fasta_record(fasta_record const &) = default;
    fasta_record(fasta_record &&) = default;
    fasta_record & operator=(fasta_record const &) = default;
    fasta_record & operator=(fasta_record &&) = default;
    ~fasta_record() = default;

    void clear() override
    {
        id_buffer.clear();
        seq_buffer.clear();
    }

protected:

    return_t id_impl() override
    {
        return id_buffer | seqan3::views::type_reduce;
    }

    return_t seq_impl() override
    {
        return seq_buffer | seqan3::views::type_reduce;
    }
};

// Default deduction guide.
format_fasta() -> format_fasta<sequence_record>;

} // namespace seqan3::awesome
