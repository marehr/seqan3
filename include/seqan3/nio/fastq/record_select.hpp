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

class field_id
{
private:

    std::string _id{};
public:

    using id_type = std::string;

    field_id() = default;
    field_id(fastq_record_raw & raw_record) : _id(raw_record.id_raw())
    {}

    id_type const & id() const { return _id; }
};

template <typename alpha_t>
class field_seq
{
public:
    using sequence_type = std::vector<alpha_t>;
private:

    sequence_type _seq{};
public:
    field_seq() = default;
    field_seq(fastq_record_raw & raw_record)
    {
        _seq.resize(raw_record.sequence_raw().size());
        std::ranges::copy(raw_record.sequence_raw() |
                          std::views::transform([] (char elem) { return assign_char_to(elem, alpha_t{}); } ),
                          _seq.begin());
    }

    sequence_type const & sequence() const { return _seq; }
};

template <typename qual_t>
class field_qual
{
public:
    using quality_sequence_type = std::vector<qual_t>;
private:

    quality_sequence_type _qual{};
public:
    field_qual() = default;
    field_qual(fastq_record_raw & raw_record)
    {
        _qual.resize(raw_record.quality_sequence_raw().size());
        std::ranges::copy(raw_record.quality_sequence_raw() |
                          std::views::transform([] (char elem) { return assign_char_to(elem, qual_t{}); } ),
                          _qual.begin());
    }

    quality_sequence_type const & quality_sequence() const { return _qual; }
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
