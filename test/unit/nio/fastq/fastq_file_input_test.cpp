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
#include <seqan3/nio/fastq/record_select.hpp>
#include <seqan3/nio/fastq/record_partial.hpp>
#include <seqan3/nio/fastq/record_proxy.hpp>
#include <seqan3/test/expect_range_eq.hpp>

using namespace seqan3::nio;

TEST(nio_fastq, file_input)
{
    fastq_file_input fin{"hello.fastq"};
    for (auto && record : fin)
    {
        std::string raw_data = std::string{record.id_raw().begin(), record.id_raw().end()}
                             + std::string{record.sequence_raw().begin(), record.sequence_raw().end()}
                             + std::string{record.quality_sequence_raw().begin(), record.quality_sequence_raw().end()};

        EXPECT_EQ(raw_data, "@SIM:1:FCX:1:15:6329:1045 1:N:0:2\n"
                            "TCGCACTCAACGCCCTGCATATGACAAGACAGAATC\n"
                            "+\n"
                            "<>;##=><9=AAAAAAAAAA9#:<#<;<<<????#=\n");
    }
}

TEST(nio_fastq, read_fastq_record)
{
    using seqan3::operator""_dna5;
    using seqan3::operator""_phred63;
    using namespace std::literals;

    fastq_file_input fin{"hello.fastq"};
    for (fastq_record<seqan3::dna5, seqan3::phred63> const & record : fin)
    {
        EXPECT_EQ(record.id(), "SIM:1:FCX:1:15:6329:1045 1:N:0:2"sv);
        EXPECT_EQ(record.sequence(), "TCGCACTCAACGCCCTGCATATGACAAGACAGAATC"_dna5);
        EXPECT_EQ(record.quality_sequence(), "<>;##=><9=AAAAAAAAAA9#:<#<;<<<????#="_phred63);
    }
}

TEST(nio_fastq, read_fastq_record_proxy)
{
    using seqan3::operator""_dna5;
    using seqan3::operator""_phred63;
    using namespace std::literals;

    fastq_file_input fin{"hello.fastq"};
    for (fastq_record_proxy<seqan3::dna5, seqan3::phred63> record : fin)
    {
        EXPECT_RANGE_EQ(record.id(), "SIM:1:FCX:1:15:6329:1045 1:N:0:2"sv);
        EXPECT_RANGE_EQ(record.sequence(), "TCGCACTCAACGCCCTGCATATGACAAGACAGAATC"_dna5);
        EXPECT_RANGE_EQ(record.quality_sequence(), "<>;##=><9=AAAAAAAAAA9#:<#<;<<<????#="_phred63);
    }
}

TEST(nio_fastq, read_fastq_record_select)
{
    using seqan3::operator""_dna5;
    using namespace std::literals;

    fastq_file_input fin{"hello.fastq"};
    for (fastq_record_select<field_id, field_seq<seqan3::dna5>> record : fin)
    {
        EXPECT_EQ(record.id(), "SIM:1:FCX:1:15:6329:1045 1:N:0:2"sv);
        EXPECT_EQ(record.sequence(), "TCGCACTCAACGCCCTGCATATGACAAGACAGAATC"_dna5);
    }
}

TEST(nio_fastq, read_fastq_record_partial)
{
    using namespace std::literals;

    // I want to modify a single entity from the record and leave everything else as is!
    fastq_file_input fin{"hello.fastq"};
    for (fastq_record_partial<field_id> record : fin)
    {
        EXPECT_EQ(record.id(), "SIM:1:FCX:1:15:6329:1045 1:N:0:2"sv);
        record.id() += std::string{" Added some cool stuff"};
        EXPECT_EQ(record.id(), "SIM:1:FCX:1:15:6329:1045 1:N:0:2 Added some cool stuff"sv); // id has modified.
        EXPECT_EQ((std::string{record.id_raw()}),
                  ">SIM:1:FCX:1:15:6329:1045 1:N:0:2 Added some cool stuff\n"sv); // id and raw are the same.
        EXPECT_EQ((std::string{record.sequence_raw().begin(), record.sequence_raw().end()}),
                  "TCGCACTCAACGCCCTGCATATGACAAGACAGAATC\n"sv); // only raw available.
        EXPECT_EQ((std::string{record.quality_sequence_raw().begin(), record.quality_sequence_raw().end()}),
                  "+\n<>;##=><9=AAAAAAAAAA9#:<#<;<<<????#=\n"sv); // only raw available.
    }
}
