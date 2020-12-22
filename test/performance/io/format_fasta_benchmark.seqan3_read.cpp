// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2020, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2020, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#include <sstream>

#include <benchmark/benchmark.h>

#include <seqan3/io/sequence_file/input.hpp>

#include "format_fasta_benchmark.hpp"
#include "format_fasta_benchmark.seqan3_read.hpp"

struct char_traits : public seqan3::sequence_file_input_default_traits_dna
{
    using sequence_alphabet = char;
    using sequence_legal_alphabet = char;
};

void seqan3_char_istringstream_read(benchmark::State & state)
{
    std::istringstream istream{fasta_file};
    seqan3::sequence_file_input<char_traits> fin{istream, seqan3::format_fasta{}};

    for (auto _ : state)
    {
        istream.clear();
        istream.seekg(0, std::ios::beg);

        auto it = fin.begin();
        for (size_t i = 0; i < iterations_per_run; ++i)
            it++;
    }

    size_t bytes_per_run = fasta_file.size();
    state.counters["iterations_per_run"] = iterations_per_run;
    state.counters["bytes_per_run"] = bytes_per_run;
    state.counters["bytes_per_second"] = seqan3::test::bytes_per_second(bytes_per_run);
}

void seqan3_dna5_istringstream_read(benchmark::State & state)
{
    std::istringstream istream{fasta_file};
    seqan3::sequence_file_input fin{istream, seqan3::format_fasta{}};

    for (auto _ : state)
    {
        istream.clear();
        istream.seekg(0, std::ios::beg);

        auto it = fin.begin();
        for (size_t i = 0; i < iterations_per_run; ++i)
            it++;
    }

    size_t bytes_per_run = fasta_file.size();
    state.counters["iterations_per_run"] = iterations_per_run;
    state.counters["bytes_per_run"] = bytes_per_run;
    state.counters["bytes_per_second"] = seqan3::test::bytes_per_second(bytes_per_run);
}
