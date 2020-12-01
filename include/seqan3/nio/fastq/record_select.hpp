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

#include <seqan3/nio/fastq/record_raw.hpp>

namespace seqan3::nio
{

class field_id
{
private:

    std::string _id{};
public:

    using id_type = std::string;

    field_id() = default;
    field_id(fastq_record_raw & raw_record) : _id{}
    {
        _id.resize(raw_record.id_transform().size());
        std::ranges::copy(raw_record.id_transform(),
                          _id.begin());
    }

    id_type const & id() const { return _id; }
};

template <typename sequence_alphabet_t>
class field_seq
{
public:
    using sequence_type = std::vector<sequence_alphabet_t>;
private:

    sequence_type _sequence{};
public:
    field_seq() = default;
    field_seq(fastq_record_raw & raw_record)
    {
        _sequence.resize(raw_record.sequence_transform().size());
        std::ranges::copy(raw_record.sequence_transform() |
                          std::views::transform([] (char elem) { return assign_char_to(elem, sequence_alphabet_t{}); } ),
                          _sequence.begin());
    }

    sequence_type const & sequence() const { return _sequence; }
};

template <typename sequence_quality_alphabet_t>
class field_qual
{
public:
    using quality_sequence_type = std::vector<sequence_quality_alphabet_t>;
private:

    quality_sequence_type _quality_sequence{};
public:
    field_qual() = default;
    field_qual(fastq_record_raw & raw_record)
    {
        _quality_sequence.resize(raw_record.quality_sequence_transform().size());
        std::ranges::copy(raw_record.quality_sequence_transform() |
                          std::views::transform([] (char elem) { return assign_char_to(elem, sequence_quality_alphabet_t{}); } ),
                          _quality_sequence.begin());
    }

    quality_sequence_type const & quality_sequence() const { return _quality_sequence; }
};

template <typename ...field_t>
class fastq_record_select : public field_t...
{
public:

    fastq_record_select() = default;
    fastq_record_select(fastq_record_raw & raw_record) : field_t{raw_record}...
    {}
};

}  // namespace seqan3::nio
