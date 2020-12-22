// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \brief Provides seqan3::alignment_record_input.hpp.
 * \author Rene Rahn <rene.rahn AT fu-berlin.de>
 */

namespace seqan3
{

// Default behaviour: noop
void record_to_alignment(...) {} // no-op

template <typename aligned_sequence_t>
class alignment_record_input
{
public:
    /*!\name Constructors, destructor and assignment
     * \{
     */
    constexpr alignment_record_input() = default; //!< Defaulted.
    constexpr alignment_record_input(alignment_record_input const & other)
    {
        // This is the reference type.
        // We might need to define a compatible value type.
        // Then the value type of the range is the copy of this one.
    }

    constexpr alignment_record_input(alignment_record_input &&) = default; //!< Defaulted.
    constexpr alignment_record_input & operator=(alignment_record_input const &) = default; //!< Defaulted.
    constexpr alignment_record_input & operator=(alignment_record_input &&) = default; //!< Defaulted.
    ~alignment_record_input() = default; //!< Defaulted.

    template <typename record_t>
    explicit constexpr alignment_record_input(record_t && record)
    {
        record_to_alignment(record, _alignment);
    }
    //!}

    auto alignment()
    {
        return _alignment.value();
    }

private:

    using alignment_type = std::optional<std::pair<aligned_sequence_t, aligned_sequence_t>>;

    alignment_type _alignment{};
};
}  // namespace seqan3
