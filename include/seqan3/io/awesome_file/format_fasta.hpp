#pragma once

#include <seqan3/std/concepts>
#include <seqan3/std/ranges>
#include <memory_resource>
#include <vector>
#include <string>

#include <seqan3/core/char_operations/predicate.hpp>
#include <seqan3/io/awesome_file/format_base.hpp>
#include <seqan3/io/awesome_file/record_sequence.hpp>
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

// TODO: Abstract into policies.
// TODO: Optimise the parsing.
template <typename record_base_t = record_sequence>
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
    //!\brief Rebinds this fasta format to a new record base type, i.e. users can extend the seqan3::awesome::record_sequence.
    template <typename new_record_base_t>
    using rebind_record = format_fasta<new_record_base_t>;

    format_fasta() = default;
    format_fasta(format_fasta const &) = default;
    format_fasta(format_fasta &&) = default;
    format_fasta & operator=(format_fasta const &) = default;
    format_fasta & operator=(format_fasta &&) = default;
    ~format_fasta() override = default;

    template <typename other_record_sequence_t>
    //!\cond
        requires (!std::same_as<other_record_sequence_t, record_base_t>)
    //!\endcond
    explicit format_fasta(format_fasta<other_record_sequence_t> other) :
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
    requires std::derived_from<base_record_t, record_sequence>
//!\endcond
class fasta_record<base_record_t> final :
    public record_sequence_wrapper<fasta_record<base_record_t>, base_record_t>,
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

    return_t id()
    {
        return id_buffer | seqan3::views::type_reduce;
    }

    return_t seq()
    {
        return seq_buffer | seqan3::views::type_reduce;
    }
};

// Default deduction guide.
format_fasta() -> format_fasta<record_sequence>;

} // namespace seqan3::awesome
