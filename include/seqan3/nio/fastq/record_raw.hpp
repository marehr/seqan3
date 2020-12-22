// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#pragma once

#include <seqan3/std/span>
#include <vector>

namespace seqan3::nio
{

class fastq_record_raw
{
public:
    // TODO: this should be an extra type memory_span{&iostream_buf (reference), absolute_position_in_file, size}
    using id_raw_type = std::span<char /*std::byte*/>;
    using sequence_raw_type = std::span<char /*std::byte*/>;
    using sequence_quality_raw_type = std::span<char /*std::byte*/>;

// private: TODO make private and explicit constructor
    id_raw_type _id_raw;
    sequence_raw_type _sequence_raw;
    sequence_quality_raw_type _quality_sequence_raw;

public:

    id_raw_type const & id_raw() const { return _id_raw; }
    sequence_raw_type const & sequence_raw() const { return _sequence_raw; }
    sequence_quality_raw_type const & quality_sequence_raw() const { return _quality_sequence_raw; }

    using segmented_memory = /*std::vector<*/std::span<char>/*>*/;

    segmented_memory id_transform(/*iostream_buf*/) // TODO: for bam formats it needs an buffer where to store memory
    {
        // TODO: this needs to do real parsing (e.g. handle \r\n or \n endings)
        // drop first byte @ and last byte \n;
        std::span id = id_raw().subspan(1, id_raw().size() - 1 - 1);
        return segmented_memory{id};
    }

    segmented_memory sequence_transform(/*iostream_buf*/) // TODO: for bam formats it needs an buffer where to store memory
    {
        // TODO: this needs to do real parsing (e.g. handle \r\n or \n endings)
        // drop last byte \n
        std::span sequence = sequence_raw().subspan(0, sequence_raw().size() - 1);
        return segmented_memory{sequence};
    }

    segmented_memory quality_sequence_transform(/*iostream_buf*/) // TODO: for bam formats it needs an buffer where to store memory
    {
        // TODO: this needs to do real parsing (e.g. handle \r\n or \n endings)
        // drop first two bytes +\n and last byte \n
        std::span quality_sequence = quality_sequence_raw().subspan(2, quality_sequence_raw().size() - 2 - 1);
        return segmented_memory{quality_sequence};
    }

    void id [[deprecated("Please write `for (fasta_record record: fasta_file)` instead of `for (auto && record: fasta_file)`; see https://docs.seqan.de/seqan/3-master-dev/io/convert")]]() = delete;
    void sequence [[deprecated("Please write `for (fasta_record record: fasta_file)` instead of `for (auto && record: fasta_file)`; see https://docs.seqan.de/seqan/3-master-dev/io/convert")]]() = delete;
    void quality_sequence [[deprecated("Please write `for (fasta_record record: fasta_file)` instead of `for (auto && record: fasta_file)`; see https://docs.seqan.de/seqan/3-master-dev/io/convert")]]() = delete;
};

}  // namespace seqan3::nio
