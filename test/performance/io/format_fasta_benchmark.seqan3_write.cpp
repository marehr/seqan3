// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2020, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2020, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#include <sstream>

#include <benchmark/benchmark.h>

#include <seqan3/io/sequence_file/output.hpp>

#include "format_fasta_benchmark.hpp"
#include "format_fasta_benchmark.seqan3_write.hpp"

void seqan3_dna5_ostringstream_write(benchmark::State & state)
{
    std::ostringstream ostream;
    seqan3::sequence_file_output fout{ostream, seqan3::format_fasta{}, seqan3::fields<seqan3::field::id,
                                                                                      seqan3::field::seq>{}};

    for (auto _ : state)
    {
        for (size_t i = 0; i < iterations_per_run; ++i)
            fout.emplace_back(fasta_seq, fasta_hdr);
    }

    ostream = std::ostringstream{};
    fout.emplace_back(fasta_seq, fasta_hdr);
    size_t bytes_per_run = ostream.str().size() * iterations_per_run;
    state.counters["iterations_per_run"] = iterations_per_run;
    state.counters["bytes_per_run"] = bytes_per_run;
    state.counters["bytes_per_second"] = seqan3::test::bytes_per_second(bytes_per_run);
}
