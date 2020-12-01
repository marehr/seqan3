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
class fastq_record_proxy : public fastq_record_raw
{
public:
    fastq_record_proxy() = delete;
    fastq_record_proxy(fastq_record_proxy const &) = delete;
    fastq_record_proxy(fastq_record_proxy &&) = default;
    fastq_record_proxy & operator=(fastq_record_proxy const &) = delete;
    fastq_record_proxy & operator=(fastq_record_proxy &&) = default;
    ~fastq_record_proxy() = default;

    fastq_record_proxy(fastq_record_raw raw_record) : fastq_record_raw{std::move(raw_record)}
    {}

    auto id() { return this->id_raw(); }
    auto sequence()
    {
        return this->sequence_raw() |
               std::views::transform([] (char elem) { return assign_char_to(elem, sequence_alphabet_t{}); });
    }

    auto quality_sequence()
    {
        return this->quality_sequence_raw() |
               std::views::transform([] (char elem) { return assign_char_to(elem, sequence_quality_alphabet_t{}); } );
    }
};

}  // namespace seqan3::nio
