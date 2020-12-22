// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2020, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2020, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/alphabet/quality/phred42.hpp>
#include <seqan3/alphabet/gap/gapped.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/formatted_file/formatted_file_input.hpp>
#include <seqan3/io/formatted_file/sequence_record_input.hpp>
#include <seqan3/io/formatted_file/alignment_record_input.hpp>
#include <seqan3/io/alignment_file/format_sam.hpp>
#include <seqan3/io/sequence_file/format_fasta.hpp>
#include <seqan3/io/sequence_file/format_fastq.hpp>

// All used parsers are the known seqan3::formats.
// Some of them are directly modified also the existing parsing was not touched at all.
// The sam format was not touched at all, but everything could be applied as a wrapper.
// This is to demonstrate that basically any external format can be easily adapted to certain
// concepts that we define. But also our formats can be adapted to be something else if the user wants to.
TEST(formatted_file_input_test, seqan3_fasta_record)
{
    std::filesystem::path file_path{"/Users/rmaerker/Development/seqan3/seqan3-build/test.fa"};

    // Now the formatted file is an sequence file that can read sequence and id from fasta.
    using sequence_record_t = seqan3::sequence_record_input<seqan3::dna5, seqan3::phred42>;
    seqan3::formatted_file_input<sequence_record_t> file{file_path, seqan3::format_fasta{}};

    for (auto && record : file)
    {
        seqan3::debug_stream << record.id() << "\n";
        seqan3::debug_stream << record.seq() << "\n";
        // seqan3::debug_stream << record.id() << "\n"; throws exception because not set by format.
    }
}

TEST(formatted_file_input_test, seqan3_fastq_record)
{
    std::filesystem::path file_path{"/Users/rmaerker/Development/seqan3/seqan3-build/test.fq"};

    // Now the formatted file is an sequence file that can read qualities from the fastq format.
    using sequence_record_t = seqan3::sequence_record_input<seqan3::dna5, seqan3::phred42>;
    seqan3::formatted_file_input<sequence_record_t> file{file_path, seqan3::format_fastq{}};

    for (auto && record : file)
    {
        seqan3::debug_stream << record.id() << "\n";
        seqan3::debug_stream << record.seq() << "\n";
        seqan3::debug_stream << record.qual() << "\n";
    }
}

TEST(formatted_file_input_test, seqan3_sam_record)
{
    std::filesystem::path file_path{"/Users/rmaerker/Development/seqan3/seqan3-build/test.sam"};

    // Now the formatted file is an sequence file even for format_sam, without touching the format at all.
    using sequence_record_t = seqan3::sequence_record_input<seqan3::dna5, seqan3::phred42>;
    seqan3::formatted_file_input<sequence_record_t> file{file_path, seqan3::format_sam_adapter{}};

    for (auto && record : file)
    {
        seqan3::debug_stream << record.id() << "\n";
        seqan3::debug_stream << record.seq() << "\n";
        seqan3::debug_stream << record.qual() << "\n";
    }
}

TEST(formatted_file_input_test, seqan3_sam_alignment_record)
{
    std::filesystem::path file_path{"/Users/rmaerker/Development/seqan3/seqan3-build/test.sam"};

    // Just prototype some reference_name to sequence map: Later this is more tightly bound to what formats provide.
    // But it is good to understand what dependencies exist.
    using reference_id_t = std::string;
    using reference_sequence_t = std::string;
    std::map<reference_id_t, reference_sequence_t> reference_id_map{};
    reference_id_map.emplace(reference_id_t{"ref"}, reference_sequence_t{"CAGTCTGATAGAGGGAGTATATA"});

    // Now the formatted file is an alignment file which can output the record interpreted as an alignment.
    using alignment_record_t = seqan3::alignment_record_input<std::vector<seqan3::gapped<seqan3::dna5>>>;
    seqan3::formatted_file_input<alignment_record_t> file{file_path, seqan3::format_sam_adapter{reference_id_map}};

    for (auto && record : file)
    {
        seqan3::debug_stream << record.alignment() << "\n";
    }
}
