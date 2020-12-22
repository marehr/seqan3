#pragma once

#include <cstring>
#include <seqan3/std/concepts>
#include <seqan3/std/ranges>
#include <vector>
#include <seqan3/std/span>
#include <string>

#include <seqan3/core/char_operations/predicate.hpp>
#include <seqan3/io/awesome_file/format_base.hpp>
#include <seqan3/io/awesome_file/record_alignment.hpp>

#include <seqan3/core/debug_stream.hpp>

namespace seqan3::awesome
{

// Header problem:
// There is a minimal header version
// This is the only one that is accessable
// If more formats have a different header, then only the common type of them can be represented.
// If a specific header is required because the information are essential, then this must be the selected header type
// and all other formats must comply with this header type. If not they cannot be represented by the same header.
// Present the central ideas from this.

// Default definition which is overloaded for the record later.
template <typename base_record_t>
class record_sam
{
    record_sam() = delete;
};

// We could offer a version type.

struct sam_header : public header_base
{
    using return_t = std::span<char>;

    std::vector<char> hd_buffer{};
    // We can have this in a fixed order.
    std::array<std::pair<size_t, size_t>, 4> slice_positions{};


    return_t version()
    {
        return hd_buffer | views::slice(slice_positions[0].first, slice_positions[0].second);
    }

    // Should return optional
    return_t sorting_order()
    {
        return hd_buffer | views::slice(slice_positions[1].first, slice_positions[1].second);
    }

    return_t sub_sorting_order()
    {
        return hd_buffer | views::slice(slice_positions[2].first, slice_positions[2].second);
    }

    return_t grouping()
    {
        return hd_buffer | views::slice(slice_positions[3].first, slice_positions[3].second);
    }

};

struct read_sam_header_line_policy
{
    void read_header_line(stream..., sam_header & header)
    {

        // steps:
        // 1. identify tag
        // 2. memcpy into respective header field.
        // 3. transform on access => simply store buffer in record and return span over the respective field.
        // 4. evaluate validity of the field?
    }
};

template <typename record_base_t = record_alignment>
//!\cond
    // requires std::semiregular<record_sam<record_base_t>>
//!\endcond
class format_sam :
    public format_base<record_base_t>,
    private read_policy,
    private skip_policy
{
private:

    using base_format_type = format_base<record_base_t>;
    using record_type = record_sam<record_base_t>;
    using typename base_format_type::istream_type;

    static constexpr auto is_id_delimiter = seqan3::is_char<'>'>;

    std::vector<std::string> valid_extensions{{"sam"}};
    record_type record{};

    template <typename>
    friend class format_sam;

    using streambuf_iterator = seqan3::detail::fast_istreambuf_iterator<char>;
    using istreambuf_type = seqan3::detail::stream_buffer_exposer<char>;
public:
    //!\brief Rebinds this fasta format to a new record base type, i.e. users can extend the seqan3::awesome::record_alignment.
    template <typename new_record_base_t>
    using rebind_record = format_sam<new_record_base_t>;

    format_sam() = default;
    format_sam(format_sam const &) = default;
    format_sam(format_sam &&) = default;
    format_sam & operator=(format_sam const &) = default;
    format_sam & operator=(format_sam &&) = default;
    ~format_sam() override = default;

    template <typename other_record_alignment_t>
    //!\cond
        requires (!std::same_as<other_record_alignment_t, record_base_t>)
    //!\endcond
    explicit format_sam(format_sam<other_record_alignment_t> other) :
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


        // record.clear();

        // assert(is_id_delimiter(*istreambuf.gptr()));

        // istreambuf.gbump(1);
        // if (!skip_until(seqan3::is_graph, istreambuf))
        //     throw std::runtime_error{"Unexpected end of input"};

        // if (!read_until(record.id_buffer, seqan3::is_char<'\n'> || seqan3::is_char<'\r'>, istreambuf))
        //     throw std::runtime_error{"Unexpected end of input"};

        // constexpr auto seq_field_delimiter = is_id_delimiter || seqan3::is_eof;
        // while (it != std::default_sentinel && !seq_field_delimiter(*it))
        // {
        //     if (!read_until(record.seq_buffer, seqan3::is_space, istreambuf))
        //         throw std::runtime_error{"Unexpected end of input"};

        //     skip_until(seqan3::is_graph, istreambuf);
        // }
        // return &record;

        assert(istream.rdbuf() != nullptr);

        istreambuf_type & istreambuf = reinterpret_cast<istreambuf_type &>(*istream.rdbuf());
        streambuf_iterator it{istreambuf};
        if (it == std::default_sentinel)
            return nullptr;

        // What are we reading here?
        auto field_view = stream_view | views::take_until_or_throw_and_consume(is_char<'\t'>);

        // these variables need to be stored to compute the ALIGNMENT
        int32_t ref_offset_tmp{};
        std::ranges::range_value_t<decltype(header.ref_ids())> ref_id_tmp{};
        [[maybe_unused]] int32_t offset_tmp{};
        [[maybe_unused]] int32_t soft_clipping_end{};
        [[maybe_unused]] std::vector<cigar> tmp_cigar_vector{};
        [[maybe_unused]] int32_t ref_length{0}, seq_length{0}; // length of aligned part for ref and query

        // Header
        // -------------------------------------------------------------------------------------------------------------

        while (is_char<'@'>(*it))
        {
            read_header_line(istreambuf, record.header); -> assert one or none and if then the first
            // @<two-letter header record type>
            // TAB-delimited lines
            // format TAG:VALUE except @CO
            // Possible matches:
            // * @HD\tcc:
            // * @SQ\tcc:
            // * @RG\tcc:
            // * @PG\tcc:
            // * @CO\t.*


            read_header(stream_view, header, ref_seqs);

            if (std::ranges::begin(stream_view) == std::ranges::end(stream_view)) // file has no records
                return;
        }

        // Fields 1-5: ID FLAG REF_ID REF_OFFSET MAPQ
        // -------------------------------------------------------------------------------------------------------------
        read_field(field_view, id);

        uint16_t flag_integral{};
        read_field(field_view, flag_integral);
        flag = sam_flag{flag_integral};

        read_field(field_view, ref_id_tmp);
        check_and_assign_ref_id(ref_id, ref_id_tmp, header, ref_seqs);

        read_field(field_view, ref_offset_tmp);
        --ref_offset_tmp; // SAM format is 1-based but SeqAn operates 0-based

        if (ref_offset_tmp == -1)
            ref_offset = std::nullopt; // indicates an unmapped read -> ref_offset is not set
        else if (ref_offset_tmp > -1)
            ref_offset = ref_offset_tmp;
        else if (ref_offset_tmp < -1)
            throw format_error{"No negative values are allowed for field::ref_offset."};

        read_field(field_view, mapq);

        // Field 6: CIGAR
        // -------------------------------------------------------------------------------------------------------------
        if constexpr (!detail::decays_to_ignore_v<align_type> || !detail::decays_to_ignore_v<cigar_type>)
        {
            if (!is_char<'*'>(*std::ranges::begin(stream_view))) // no cigar information given
            {
                std::tie(tmp_cigar_vector, ref_length, seq_length) = parse_cigar(field_view);
                transfer_soft_clipping_to(tmp_cigar_vector, offset_tmp, soft_clipping_end);
                // the actual cigar_vector is swapped with tmp_cigar_vector at the end to avoid copying
            }
            else
            {
                std::ranges::next(std::ranges::begin(field_view)); // skip '*'
            }
        }
        else
        {
            detail::consume(field_view);
        }

        offset = offset_tmp;

        // Field 7-9: (RNEXT PNEXT TLEN) = MATE
        // -------------------------------------------------------------------------------------------------------------
        if constexpr (!detail::decays_to_ignore_v<mate_type>)
        {
            std::ranges::range_value_t<decltype(header.ref_ids())> tmp_mate_ref_id{};
            read_field(field_view, tmp_mate_ref_id); // RNEXT

            if (tmp_mate_ref_id == "=") // indicates "same as ref id"
            {
                if constexpr (!detail::decays_to_ignore_v<ref_id_type>)
                    get<0>(mate) = ref_id;
                else
                    check_and_assign_ref_id(get<0>(mate), ref_id_tmp, header, ref_seqs);
            }
            else
            {
                check_and_assign_ref_id(get<0>(mate), tmp_mate_ref_id, header, ref_seqs);
            }

            int32_t tmp_pnext{};
            read_field(field_view, tmp_pnext); // PNEXT

            if (tmp_pnext > 0)
                get<1>(mate) = --tmp_pnext; // SAM format is 1-based but SeqAn operates 0-based.
            else if (tmp_pnext < 0)
                throw format_error{"No negative values are allowed at the mate mapping position."};
            // tmp_pnext == 0 indicates an unmapped mate -> do not fill std::optional get<1>(mate)

            read_field(field_view, get<2>(mate)); // TLEN
        }
        else
        {
            for (size_t i = 0; i < 3u; ++i)
            {
                detail::consume(field_view);
            }
        }

        // Field 10: Sequence
        // -------------------------------------------------------------------------------------------------------------
        if (!is_char<'*'>(*std::ranges::begin(stream_view))) // sequence information is given
        {
            auto constexpr is_legal_alph = is_in_alphabet<seq_legal_alph_type>;
            auto seq_stream = field_view | std::views::transform([is_legal_alph] (char const c) // enforce legal alphabet
                                        {
                                            if (!is_legal_alph(c))
                                                throw parse_error{std::string{"Encountered an unexpected letter: "} +
                                                                    is_legal_alph.msg +
                                                                    " evaluated to false on " +
                                                                    detail::make_printable(c)};
                                            return c;
                                        });

            if constexpr (detail::decays_to_ignore_v<seq_type>)
            {
                if constexpr (!detail::decays_to_ignore_v<align_type>)
                {
                    static_assert(sequence_container<std::remove_reference_t<decltype(get<1>(align))>>,
                                "If you want to read ALIGNMENT but not SEQ, the alignment"
                                " object must store a sequence container at the second (query) position.");

                    if (!tmp_cigar_vector.empty()) // only parse alignment if cigar information was given
                    {

                        auto tmp_iter = std::ranges::begin(seq_stream);
                        std::ranges::advance(tmp_iter, offset_tmp);

                        for (; seq_length > 0; --seq_length) // seq_length is not needed anymore
                        {
                            get<1>(align).push_back(std::ranges::range_value_t<decltype(get<1>(align))>{}.assign_char(*tmp_iter));
                            ++tmp_iter;
                        }

                        std::ranges::advance(tmp_iter, soft_clipping_end);
                    }
                    else
                    {
                        get<1>(align) = std::remove_reference_t<decltype(get<1>(align))>{}; // empty container
                    }
                }
                else
                {
                    detail::consume(seq_stream);
                }
            }
            else
            {
                read_field(seq_stream, seq);

                if constexpr (!detail::decays_to_ignore_v<align_type>)
                {
                    if (!tmp_cigar_vector.empty()) // if no alignment info is given, the field::alignment should remain empty
                    {
                        assign_unaligned(get<1>(align),
                                        seq | views::slice(static_cast<decltype(std::ranges::size(seq))>(offset_tmp),
                                                        std::ranges::size(seq) - soft_clipping_end));
                    }
                }
            }
        }
        else
        {
            std::ranges::next(std::ranges::begin(field_view)); // skip '*'
        }

        // Field 11:  Quality
        // -------------------------------------------------------------------------------------------------------------
        auto const tab_or_end = is_char<'\t'> || is_char<'\r'> || is_char<'\n'>;
        read_field(stream_view | views::take_until_or_throw(tab_or_end), qual);

        if constexpr (!detail::decays_to_ignore_v<seq_type> && !detail::decays_to_ignore_v<qual_type>)
        {
            if (std::ranges::distance(seq) != 0 && std::ranges::distance(qual) != 0 &&
                std::ranges::distance(seq) != std::ranges::distance(qual))
            {
                throw format_error{detail::to_string("Sequence length (", std::ranges::distance(seq),
                                                    ") and quality length (", std::ranges::distance(qual),
                                                    ") must be the same.")};
            }
        }

        // All remaining optional fields if any: SAM tags dictionary
        // -------------------------------------------------------------------------------------------------------------
        while (is_char<'\t'>(*std::ranges::begin(stream_view))) // read all tags if present
        {
            std::ranges::next(std::ranges::begin(stream_view)); // skip tab
            read_field(stream_view | views::take_until_or_throw(tab_or_end), tag_dict);
        }

        detail::consume(stream_view | views::take_until(!(is_char<'\r'> || is_char<'\n'>))); // consume new line

        // DONE READING - wrap up
        // -------------------------------------------------------------------------------------------------------------
        // Alignment object construction
        // Note that the query sequence in get<1>(align) has already been filled while reading Field 10.
        if constexpr (!detail::decays_to_ignore_v<align_type>)
        {
            int32_t ref_idx{(ref_id_tmp.empty()/*unmapped read?*/) ? -1 : 0};

            if constexpr (!detail::decays_to_ignore_v<ref_seqs_type>)
            {
                if (!ref_id_tmp.empty())
                {
                    assert(header.ref_dict.count(ref_id_tmp) != 0); // taken care of in check_and_assign_ref_id()
                    ref_idx = header.ref_dict[ref_id_tmp];          // get index for reference sequence
                }
            }

            construct_alignment(align, tmp_cigar_vector, ref_idx, ref_seqs, ref_offset_tmp, ref_length);
        }

        if constexpr (!detail::decays_to_ignore_v<cigar_type>)
            std::swap(cigar_vector, tmp_cigar_vector);
    }
};

// ----------------------------------------------------------------------------
// record_sam implementation.
// ----------------------------------------------------------------------------

// We can solve the problem of multiple inheritance

template <typename base_record_t>
//!\cond
    requires std::derived_from<base_record_t, record_alignment>
//!\endcond
class record_sam<base_record_t> final :
    public base_record_t,
    private record_registration_policy<record_sam<base_record_t>>
{
private:
    friend record_registration_policy<record_sam<base_record_t>>;

    template <typename>
    friend class format_sam;

    using typename base_record_t::return_t;

    std::vector<char> id_buffer{};
    std::vector<char> seq_buffer{};
public:

    using buffer_interval_type = std::pair<size_t, size_t>;

    record_sam() = default;
    record_sam(record_sam const &) = default;
    record_sam(record_sam &&) = default;
    record_sam & operator=(record_sam const &) = default;
    record_sam & operator=(record_sam &&) = default;
    ~record_sam() = default;

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
format_sam() -> format_sam<record_alignment>;

} // namespace seqan3::awesome

} // namespace seqan3::awesome
