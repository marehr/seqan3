#include <gtest/gtest.h>

#include <sstream>

#include <seqan3/io/awesome_file/file_base.hpp>
#include <seqan3/io/awesome_file/sequence_file_in.hpp>
#include <seqan3/io/awesome_file/format_fasta.hpp>
#include <seqan3/io/awesome_file/record_get_adaptor.hpp>
#include <seqan3/io/awesome_file/decorated_file.hpp>

#include <seqan3/core/debug_stream.hpp>

namespace my_test
{

enum struct my_field
{
    ext_id
    // add more
};

class my_record_sequence : public seqan3::awesome::record_sequence
{
private:
    my_record_sequence * derived{};

    using base_t = seqan3::awesome::record_sequence;
public:
    using typename base_t::return_t;

    my_record_sequence() = default;
    my_record_sequence(my_record_sequence const &) = default;
    my_record_sequence(my_record_sequence &&) = default;
    my_record_sequence & operator=(my_record_sequence const &) = default;
    my_record_sequence & operator=(my_record_sequence &&) = default;
    virtual ~my_record_sequence() = default;

    return_t ext_id() const
    {
        assert(derived != nullptr);
        return derived->ext_id_impl();
    }

    using field_to_function_map =
        seqan3::list_traits::concat<typename base_t::field_to_function_map,
                                    seqan3::type_list<seqan3::awesome::field_to_function<my_field::ext_id,
                                                                                         &my_record_sequence::ext_id>
                                    >
        >;

protected:
    virtual return_t ext_id_impl()
    {
        throw std::runtime_error{"The ext id was not set by the format."};
        return return_t{};
    }

    void register_record(my_record_sequence * derived) noexcept
    {
        this->derived = derived;
        base_t::register_record(derived);
    }
};

template <typename derived_t, typename my_base_record_t>
    requires std::derived_from<my_base_record_t, my_record_sequence>
class my_record_sequence_wrapper : public my_base_record_t
{
private:
    friend derived_t;

    using typename my_base_record_t::return_t;

    // TODO: Inherit these interfaces!
    return_t id_impl() override
    {
        return static_cast<derived_t *>(this)->id();
    }

    return_t seq_impl() override
    {
        return static_cast<derived_t *>(this)->seq();
    }

    return_t qual_impl() override
    {
        return static_cast<derived_t *>(this)->qual();
    }

    return_t ext_id_impl() override
    {
        return static_cast<derived_t *>(this)->ext_id();
    }
};

template <typename base_record_t>
    requires std::derived_from<base_record_t, my_record_sequence>
class ext_fasta_record final :
    public base_record_t,
    // public my_record_sequence_wrapper<ext_fasta_record<base_record_t>, base_record_t>,
    private seqan3::awesome::record_registration_policy<ext_fasta_record<base_record_t>>
{
private:

    friend seqan3::awesome::record_registration_policy<ext_fasta_record<base_record_t>>;
    using typename base_record_t::return_t;
public:

    using buffer_interval_type = std::pair<size_t, size_t>;

    // // Might have a header pointer.
    std::vector<char> buffer{};
    std::vector<char> second_id{};
    std::vector<buffer_interval_type> buffer_field_positions{3 , {0, 0}};

    ext_fasta_record() = default;
    ext_fasta_record(ext_fasta_record const &) = default;
    ext_fasta_record(ext_fasta_record &&) = default;
    ext_fasta_record & operator=(ext_fasta_record const &) = default;
    ext_fasta_record & operator=(ext_fasta_record &&) = default;
    ~ext_fasta_record() = default;

    void clear() override
    {
        buffer.clear();
        second_id.clear();
        std::ranges::fill(buffer_field_positions, buffer_interval_type{0, 0});
    }

    return_t id_impl() override
    {
        int32_t field_pos = static_cast<int32_t>(seqan3::awesome::field::id);
        return buffer | seqan3::views::slice(buffer_field_positions[field_pos].first,
                                             buffer_field_positions[field_pos].second);
    }

    return_t seq_impl() override
    {
        int32_t field_pos = static_cast<int32_t>(seqan3::awesome::field::seq);
        return buffer | seqan3::views::slice(buffer_field_positions[field_pos].first,
                                             buffer_field_positions[field_pos].second);
    }

    return_t ext_id_impl() override
    {
        return second_id | seqan3::views::slice(0, second_id.size());
    }
};

template <typename record_base_t = my_record_sequence>
    requires std::semiregular<ext_fasta_record<record_base_t>>
class ext_format_fasta final : public seqan3::awesome::format_base<record_base_t>
{
private:

    using base_format_type = seqan3::awesome::format_base<record_base_t>;
    using record_type = ext_fasta_record<record_base_t>;
    using typename base_format_type::istream_type;
    using streambuf_iterator = seqan3::detail::fast_istreambuf_iterator<char>;

    static constexpr auto is_id_delimiter = seqan3::is_char<'>'>;

    std::vector<std::string> valid_extensions{{"fastx"}};
    record_type record{};

    template <typename>
    friend class ext_format_fasta;

public:
    //!\brief Rebinds this fasta format to a new record base type, i.e. users can extend the seqan3::awesome::record_sequence.
    template <typename new_record_base_t>
    using rebind_record = ext_format_fasta<new_record_base_t>;

    ext_format_fasta() = default;
    ext_format_fasta(ext_format_fasta const & other) = default;
    ext_format_fasta(ext_format_fasta &&) = default;
    ext_format_fasta & operator=(ext_format_fasta const &) = default;
    ext_format_fasta & operator=(ext_format_fasta &&) = default;
    ~ext_format_fasta() override = default;

    template <typename other_record_sequence_t>
    explicit ext_format_fasta(ext_format_fasta<other_record_sequence_t> other) :
        valid_extensions{std::move(other.valid_extensions)}
    {
        seqan3::debug_stream << "ex_format_fasta: Calling converting constructor!\n";
    }

    std::vector<std::string> & extensions() override
    {
        return valid_extensions;
    }

    record_type * read_record(istream_type & istream) override
    {
        streambuf_iterator it{*istream.rdbuf()};
        if (it == std::default_sentinel)
            return nullptr;

        record.clear();
        // Also just parse repeatedly, until delimiter is reached and adding to the buffer
        // And afterwards set the id.
        assert(is_id_delimiter(*it));

        size_t buffer_position = 0;
        ++it;
        skip_until(it, seqan3::is_graph); // Until the first non space character.

        buffer_position += read_until(it, record.buffer, seqan3::is_char<'\n'> || seqan3::is_char<'\r'>);
        set_buffer_position(record, seqan3::awesome::field::id, {0, buffer_position});

        skip_until(it, is_id_delimiter); // Until the next id
        ++it;
        skip_until(it, seqan3::is_graph); // Until the first non space character.
        read_until(it, record.second_id, seqan3::is_char<'\n'> || seqan3::is_char<'\r'>);

        auto seq_field_delimiter = is_id_delimiter || seqan3::is_eof;
        size_t seq_buffer_begin = buffer_position;

        while (it != std::default_sentinel && !seq_field_delimiter(*it))
        {
            buffer_position += read_until(it, record.buffer, seqan3::is_space);
            skip_until(it, seqan3::is_graph);
        }
        set_buffer_position(record, seqan3::awesome::field::seq, {seq_buffer_begin, buffer_position});

        // seqan3::debug_stream << record.buffer << "\n";
        // seqan3::debug_stream << record.second_id << "\n";
        // seqan3::debug_stream << record.buffer_field_positions << "\n";
        return &record;
    }

private:
    template <typename delimiter_t, typename buffer_type>
    size_t read_until(streambuf_iterator & it, buffer_type & buffer, delimiter_t && delimiter)
    {
        // Now we want to actually parse the record.
        // We actually run over the stream and delimit the field.
        // We might be able to also provide some skip characters? Or we can do this later.
        // When we actually apply the conversion.

        size_t current_data_size = buffer.size();

        // We risk elementwise copy here, but want probably some optimisations, like memcpy.
        // So we could scan until the delimter and read in the chunk.
        // std::ranges::copy_if(it, streambuf_iterator{}, std::cpp20::back_inserter(record.buffer), !delimiter);
        for (; it != std::default_sentinel && !delimiter(*it); ++it)
            buffer.push_back(*it);

        return buffer.size() - current_data_size;
    }

    template <typename delimiter_t>
    void skip_until(streambuf_iterator & streambuf_it, delimiter_t && delimiter)
    {
        for (; streambuf_it != std::default_sentinel && !delimiter(*streambuf_it); ++streambuf_it)
        {}
    }

    void set_buffer_position(record_type & record,
                             seqan3::awesome::field const field_id,
                             std::pair<size_t, size_t> buffer_pos)
    {
        record.buffer_field_positions[static_cast<int32_t>(field_id)] = std::move(buffer_pos);
    }

};

// Default deduction guide.
ext_format_fasta() -> ext_format_fasta<>;

} // namespace my_test

// TEST(awesome_file_test, construct)
// {
//     using fasta_t = seqan3::awesome::format_fasta<>;
//     using ext_fasta_t = my_test::ext_format_fasta<>;

//     fasta_t fasta_fmt{};
//     fasta_fmt.extensions().push_back("foo");
//     ext_fasta_t ext_fasta_fmt{};

//     using base_record_t = my_test::my_record_sequence;
//     using base_format_t = seqan3::awesome::format_base<base_record_t>;
//     using unique_ptr_t = std::unique_ptr<base_format_t>;
//     std::vector<unique_ptr_t> formats{};

//     formats.push_back(std::make_unique<typename fasta_t::rebind_record<base_record_t>>(fasta_fmt));
//     formats.push_back(std::make_unique<ext_fasta_t>(ext_fasta_fmt));

//     // base_format_t * base_fmt = &ext_fasta_fmt;
//     base_format_t * base_fmt = formats[1].get();

//     std::string record = "> ID1\n> ID1 -bla\nACGTTTTTTTTTTTTTTT\n";
//     std::istringstream strm{record};
//     seqan3::detail::fast_istreambuf_iterator<char> it{*strm.rdbuf()};

//     auto && fa_ref = base_fmt->read_record(it);

//     seqan3::debug_stream << "ID: " << fa_ref.id() << "\n";
//     seqan3::debug_stream << "SEQ: " << fa_ref.seq() << "\n";
//     seqan3::debug_stream << "Ext ID: " << fa_ref.ext_id() << "\n";
// }

TEST(awesome_file_test, rebind)
{
    std::filesystem::path p{"/Users/rmaerker/Development/seqan3/seqan3-build/test.fa"};

    using file_t = seqan3::awesome::input_file<my_test::my_record_sequence>;
    file_t in_file{p, seqan3::awesome::format_fasta{}, my_test::ext_format_fasta{}};

    for (auto && record : in_file)
    {
        // seqan3::debug_stream << "Ext ID:  " << record.ext_id() << "\n";
        seqan3::debug_stream << "ID:  " << record.id() << "\n";
        seqan3::debug_stream << "Seq: " << record.seq() << "\n";  // Pure byte string.
    }
}

TEST(awesome_file_test, interpret)
{

    std::filesystem::path p{"/Users/rmaerker/Development/seqan3/seqan3-build/test.fa"};

    {
        seqan3::awesome::sequence_file_in in_file{p, seqan3::awesome::format_fasta{}};
        static_assert(std::ranges::range<seqan3::awesome::sequence_file_in>);
        static_assert(std::ranges::input_range<seqan3::awesome::sequence_file_in>);
        // auto it = in_file.begin();

        for (auto && record : in_file)
        {
            seqan3::debug_stream << "ID:  " << record.id() << "\n";
            seqan3::debug_stream << "Seq: " << record.seq() << "\n";  // Pure byte string.
        }
    }
    // What if I want transformation?
    // record.template seq<std::vector<seqan3::dna4>>();
}

TEST(awesome_file_test, get_decorator)
{
    std::filesystem::path p{"/Users/rmaerker/Development/seqan3/seqan3-build/test.fa"};

    seqan3::awesome::sequence_file_in in_file{p, seqan3::awesome::format_fasta{}};
    using v_t = std::ranges::range_value_t<decltype(in_file)>;
    using decorated_record_t = seqan3::awesome::record_get_adaptor<v_t, seqan3::awesome::field::seq, seqan3::awesome::field::id>;
    // We want to add
    for (decorated_record_t && record : in_file)
    {
        auto && [seq, id] = record;
        seqan3::debug_stream << "ID:" << id << "\n";
        seqan3::debug_stream << "Seq:" << seq << "\n";
    }
}

TEST(awesome_file_test, decorated_file)
{

    std::filesystem::path p{"/Users/rmaerker/Development/seqan3/seqan3-build/test.fa"};

    using file_t = seqan3::awesome::decorated_file<seqan3::awesome::sequence_file_in,
                                                   seqan3::awesome::field::seq>;
    file_t in_file{seqan3::awesome::sequence_file_in{p, seqan3::awesome::format_fasta{}}};

    for (auto && [seq] : in_file)
    {
        // seqan3::debug_stream << "ID:  " << id << "\n";
        seqan3::debug_stream << "Seq: " << seq << "\n";  // Pure byte string.
    }
}
