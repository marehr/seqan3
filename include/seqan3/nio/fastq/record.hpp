// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#pragma once

#include <seqan3/std/algorithm>
#include <string>
#include <vector>
#include <seqan3/std/ranges>

#include <seqan3/alphabet/concept.hpp>

namespace seqan3::nio
{

template <writable_alphabet sequence_alphabet_t, writable_alphabet sequence_quality_alphabet_t>
class fastq_record
{
public:
    using id_type = std::string;
    using sequence_type = std::vector<sequence_alphabet_t>;
    using sequence_quality_type = std::vector<sequence_quality_alphabet_t>;

private:
    id_type _id;
    sequence_type _sequence;
    sequence_quality_type _quality_sequence;
public:
    fastq_record() = default;
    fastq_record(fastq_record_raw & raw_record)
    {
        _id = raw_record.id_raw();
        _sequence.resize(raw_record.sequence_raw().size());
        std::ranges::copy(raw_record.sequence_raw() |
                          std::views::transform([] (char elem) { return assign_char_to(elem, sequence_alphabet_t{}); } ),
                          _sequence.begin());

        _quality_sequence.resize(raw_record.quality_sequence_raw().size());
        std::ranges::copy(raw_record.quality_sequence_raw() |
                          std::views::transform([] (char elem) { return assign_char_to(elem, sequence_quality_alphabet_t{}); } ),
                          _quality_sequence.begin());
    }

    id_type const & id() const { return _id; }
    sequence_type const & sequence() const { return _sequence; }
    sequence_quality_type const & quality_sequence() const { return _quality_sequence; }
};

}  // namespace seqan3::nio
