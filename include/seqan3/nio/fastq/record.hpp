// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#pragma once

#include <seqan3/nio/fastq/record_select.hpp>

namespace seqan3::nio
{

template <writable_alphabet sequence_alphabet_t, writable_alphabet sequence_quality_alphabet_t>
class fastq_record :
    public fastq_record_select<field_id, field_seq<sequence_alphabet_t>, field_qual<sequence_quality_alphabet_t>>
{
private:
    using base_t =
        fastq_record_select<field_id, field_seq<sequence_alphabet_t>, field_qual<sequence_quality_alphabet_t>>;
public:
    using id_type = typename field_id::id_type;
    using sequence_type = typename field_seq<sequence_alphabet_t>::sequence_type;
    using sequence_quality_type = typename field_qual<sequence_quality_alphabet_t>::quality_sequence_type;

    fastq_record() = default;
    fastq_record(fastq_record_raw & raw_record) : base_t{raw_record}
    {}
};

}  // namespace seqan3::nio
