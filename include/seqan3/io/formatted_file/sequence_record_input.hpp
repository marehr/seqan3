// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \brief Provides seqan3::sequence_record_input.hpp.
 * \author Rene Rahn <rene.rahn AT fu-berlin.de>
 */

#pragma once

#include <optional>
#include <seqan3/std/span>

#include <seqan3/range/views/char_to.hpp>

namespace seqan3
{

// Default behaviour: noop
void record_to_id_field(...) {} // no-op

void record_to_seq_field(...) {} // no-op

void record_to_qual_field(...) {} // no-op

// Default overload if certain information are present in the record.
// Allows us to provide the customisation for our type only at one place.
// TODO: This needs more requirements, but this comes later.
template <typename record_t, typename id_type>
    requires requires (record_t const & r) { {r.id_field}; }
void record_to_id_field(record_t const & record, id_type & id_field)
{
    id_field = std::span{record.id_field};
}

template <typename record_t, typename id_type>
    requires requires (record_t const & r) { {r.seq_field}; }
void record_to_seq_field(record_t const & record, id_type & seq_field)
{
    seq_field = std::span{record.seq_field};
}

template <typename record_t, typename id_type>
    requires requires (record_t const & r) { {r.qual_field}; }
void record_to_qual_field(record_t const & record, id_type & qual_field)
{
    qual_field = std::span{record.qual_field};
}

template <typename seq_type, typename qual_type>
class sequence_record_input
{
public:
    /*!\name Constructors, destructor and assignment
     * \{
     */
    constexpr sequence_record_input() = default; //!< Defaulted.
    constexpr sequence_record_input(sequence_record_input const & other)
    {
        // This is the reference type, if copied the referenced data must be materialised somewhere.
        // Could define a value_type that materialises the data instead.
    }

    constexpr sequence_record_input(sequence_record_input &&) = default; //!< Defaulted.
    constexpr sequence_record_input & operator=(sequence_record_input const &) = default; //!< Defaulted.
    constexpr sequence_record_input & operator=(sequence_record_input &&) = default; //!< Defaulted.
    ~sequence_record_input() = default; //!< Defaulted.

    template <typename record_t>
    explicit constexpr sequence_record_input(record_t && record)
    {
        record_to_id_field(record, id_field);
        record_to_seq_field(record, seq_field);
        record_to_qual_field(record, qual_field);
    }
    //!}

    auto id()
    {
        return id_field.value();
    }

    auto seq()
    {
        return seq_field.value() | views::char_to<seq_type>;
    }

    auto qual()
    {
        return qual_field.value() | views::char_to<qual_type>;
    }

private:

    using field_type = std::optional<std::span<char const>>;

    field_type id_field{std::nullopt};
    field_type seq_field{std::nullopt};
    field_type qual_field{std::nullopt};
};
}  // namespace seqan3
