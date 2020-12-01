// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>


#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/alphabet/quality/phred63.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/nio/fastq/fastq_file_input.hpp>
#include <seqan3/nio/fastq/record.hpp>
#include <seqan3/nio/fastq/record_proxy.hpp>

using namespace seqan3::nio;

TEST(nio_fastq, file_input)
{
    fastq_file_input fin{"hello.fasta"};
}

TEST(nio_fastq, read_fastq_record)
{
    fastq_file_input fin{"hello.fasta"};
    for (fastq_record<seqan3::dna5, seqan3::phred63> const & record : fin)
    {
        seqan3::debug_stream << record.id() << "\n"
                             << record.sequence() << "\n"
                             << record.quality_sequence() << "\n";
    }
}

TEST(nio_fastq, read_fastq_record_proxy)
{
    fastq_file_input fin{"hello.fasta"};
    for (fastq_record_proxy<seqan3::dna5, seqan3::phred63> record : fin)
    {
        seqan3::debug_stream << record.id() << "\n"
                             << record.sequence() << "\n"
                             << record.quality_sequence() << "\n";
    }
}
