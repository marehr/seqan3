// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2020, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2020, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#include <sstream>

#include <benchmark/benchmark.h>

#include <seqan/seq_io.h>

#include "format_fasta_benchmark.hpp"
#include "format_fasta_benchmark.seqan2_read.hpp"

#if SEQAN3_HAS_SEQAN2

#include <seqan/seq_io.h>

void seqan2_char_istringstream_read(benchmark::State & state)
{
    seqan::CharString id;
    seqan::CharString seq;

    std::istringstream istream{fasta_file};

    for (auto _ : state)
    {
        istream.clear();
        istream.seekg(0, std::ios::beg);
        auto it = seqan::Iter<std::istringstream, seqan::StreamIterator<seqan::Input> >(istream);

        for (size_t i = 0; i < iterations_per_run; ++i)
        {
            readRecord(id, seq, it, seqan::Fasta{});
            clear(id);
            clear(seq);
        }
    }

    size_t bytes_per_run = fasta_file.size();
    state.counters["iterations_per_run"] = iterations_per_run;
    state.counters["bytes_per_run"] = bytes_per_run;
    state.counters["bytes_per_second"] = seqan3::test::bytes_per_second(bytes_per_run);
}

void seqan2_dna5_istringstream_read(benchmark::State & state)
{
    seqan::CharString id;
    seqan::Dna5String seq;

    std::istringstream istream{fasta_file};

    for (auto _ : state)
    {
        istream.clear();
        istream.seekg(0, std::ios::beg);
        auto it = seqan::Iter<std::istringstream, seqan::StreamIterator<seqan::Input> >(istream);

        for (size_t i = 0; i < iterations_per_run; ++i)
        {
            readRecord(id, seq, it, seqan::Fasta{});
            clear(id);
            clear(seq);
        }
    }

    size_t bytes_per_run = fasta_file.size();
    state.counters["iterations_per_run"] = iterations_per_run;
    state.counters["bytes_per_run"] = bytes_per_run;
    state.counters["bytes_per_second"] = seqan3::test::bytes_per_second(bytes_per_run);
}
#endif // SEQAN3_HAS_SEQAN2
