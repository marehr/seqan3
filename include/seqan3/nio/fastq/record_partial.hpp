// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#pragma once

#include <seqan3/core/type_list/traits.hpp>
#include <seqan3/nio/fastq/record_raw.hpp>
#include <seqan3/range/views/to_char.hpp>

namespace seqan3::nio
{

// We can well document the concept of the field
// Every file group can have its own field concepts
// That makes it easy to reason about the code and it can be well tested.
template <typename field_t>
SEQAN3_CONCEPT id_field = requires ()
{
    typename field_t::id_type;
};

template <typename field_t>
SEQAN3_CONCEPT sequence_field = requires ()
{
    typename field_t::sequence_type;
};

template <typename field_t>
SEQAN3_CONCEPT quality_sequence_field = requires ()
{
    typename field_t::quality_sequence_type;
};

template <typename ...fields_t>
class fastq_selected_fields : public fields_t...
{
private:
    template <typename field_t>
    using is_id_type = std::bool_constant<id_field<field_t>>;

    template <typename field_t>
    using is_sequence_type = std::bool_constant<sequence_field<field_t>>;

    template <typename field_t>
    using is_quality_sequence_type = std::bool_constant<quality_sequence_field<field_t>>;

    static constexpr std::ptrdiff_t id_field_position = pack_traits::find_if<is_id_type, fields_t...>;
    static constexpr std::ptrdiff_t sequence_field_position = pack_traits::find_if<is_sequence_type, fields_t...>;
    static constexpr std::ptrdiff_t quality_sequence_field_position = pack_traits::find_if<is_quality_sequence_type, fields_t...>;

protected:

    static constexpr bool has_id_field = id_field_position != -1;
    static constexpr bool has_sequence_field = sequence_field_position != -1;
    static constexpr bool has_quality_sequence_field = quality_sequence_field_position != -1;

    fastq_selected_fields() = default;
    fastq_selected_fields(fastq_record_raw & raw_record) : fields_t{raw_record}...
    {}

    static auto id_raw(pack_traits::at<id_field_position, fields_t...> const & field_id)
        requires has_id_field
    {
        std::string tmp{">"};
        tmp += field_id.id();
        tmp += std::string{"\n"};
        return tmp;  // raw memory lives somewhere else.
    }

    static auto sequence_raw(fastq_selected_fields const & fields)
        requires has_sequence_field
    {
        std::vector<char> tmp_sequence{};
        tmp_sequence.resize(fields.sequence().size());
        std::ranges::copy(fields.sequence() | views::to_char, tmp_sequence.begin());

        return tmp_sequence;
    }

    static auto quality_sequence_raw(fastq_selected_fields const & fields)
    {
        std::vector<char> tmp_sequence{};
        tmp_sequence.resize(fields.quality_sequence().size());
        std::ranges::copy(fields.quality_sequence() | views::to_char, tmp_sequence.begin());

        return tmp_sequence;
    }

    static auto & id(fastq_selected_fields & fields)
        requires has_id_field
    {
        static_assert(id_field_position != -1);

        using field_id_t = pack_traits::at<id_field_position, fields_t...>;
        return static_cast<field_id_t &>(fields).id();
    }

    static auto const & id(fastq_selected_fields const & fields)
        requires has_id_field
    {
        static_assert(id_field_position != -1);

        using field_id_t = pack_traits::at<id_field_position, fields_t...>;
        return static_cast<field_id_t const &>(fields).id();
    }

    static auto & sequence(fastq_selected_fields & fields)
        requires has_sequence_field
    {
        return fields.sequence();
    }

    static auto const & sequence(fastq_selected_fields const & fields)
        requires has_sequence_field
    {
        return fields.sequence();
    }

    static auto & quality_sequence(fastq_selected_fields & fields)
        requires has_quality_sequence_field
    {
        return fields.quality_sequence();
    }

    static auto const & quality_sequence(fastq_selected_fields const & fields)
        requires has_quality_sequence_field
    {
        return fields.quality_sequence();
    }
};

// TODO: Require the fields! Maybe not different entities but put together into a single class.
template <typename ...fields_t>
class fastq_record_partial : protected fastq_selected_fields<fields_t...>
{
private:
    using base_t = fastq_selected_fields<fields_t...>;

    // Captures the original raw record.
    fastq_record_raw & raw_record;

public:

    fastq_record_partial() = delete;
    fastq_record_partial(fastq_record_partial const &) = delete;
    fastq_record_partial(fastq_record_partial &&) = default;
    fastq_record_partial & operator=(fastq_record_partial const &) = delete;
    fastq_record_partial & operator=(fastq_record_partial &&) = default;
    ~fastq_record_partial() = default;
    fastq_record_partial(fastq_record_raw & raw_record) : base_t{raw_record}, raw_record{raw_record}
    {}

    // Here we expect a concept that works like the following:
    decltype(auto) id_raw() const
        requires base_t::has_id_field
    {
        return base_t::id_raw(*this);
    }

    decltype(auto) id_raw() const
        requires (!base_t::has_id_field)
    {
        return raw_record.id_raw();
    }

    decltype(auto) sequence_raw() const
        requires base_t::has_sequence_field
    {
        return base_t::sequence_raw(*this);
    }

    decltype(auto) sequence_raw() const
        requires (!base_t::has_sequence_field)
    {
        return raw_record.sequence_raw();
    }

    decltype(auto) quality_sequence_raw() const
        requires base_t::has_quality_sequence_field
    {
        return base_t::sequence_quality_raw(*this);
    }

    decltype(auto) quality_sequence_raw() const
        requires (!base_t::has_quality_sequence_field)
    {
        return raw_record.quality_sequence_raw();
    }

    // Here we expect a concept that works like the following:
    auto & id()
        requires base_t::has_id_field
    {
        return base_t::id(*this);
    }

    auto const & id() const
        requires base_t::has_id_field
    {
        return base_t::id(*this);
    }

    auto & sequence()
        requires base_t::has_sequence_field
    {
        return base_t::sequence(*this);
    }

    auto const & sequence() const
        requires base_t::has_sequence_field
    {
        return base_t::sequence(*this);
    }

    auto & quality_sequence()
        requires base_t::has_quality_sequence_field
    {
        return base_t::quality_sequence(*this);
    }

    auto const & quality_sequence() const
        requires base_t::has_quality_sequence_field
    {
        return base_t::quality_sequence(*this);
    }
};
}  // namespace seqan3::nio
