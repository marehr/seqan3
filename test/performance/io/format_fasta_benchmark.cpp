// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2020, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2020, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#include "format_fasta_benchmark.hpp"

#include "format_fasta_benchmark.seqan2_read.hpp"
#include "format_fasta_benchmark.seqan2_write.hpp"
#include "format_fasta_benchmark.seqan3_read.hpp"
#include "format_fasta_benchmark.seqan3_write.hpp"
#include "format_fasta_benchmark.seqan3_nio_read.cpp"

BENCHMARK(seqan3_dna5_ostringstream_write);

#ifdef SEQAN3_HAS_SEQAN2
BENCHMARK(seqan2_dna5_ostringstream_write);
#endif // SEQAN3_HAS_SEQAN2

BENCHMARK(seqan3_char_istringstream_read);
BENCHMARK(seqan3_dna5_istringstream_read);
BENCHMARK(seqan3_nio_char_read);

#ifdef SEQAN3_HAS_SEQAN2
BENCHMARK(seqan2_char_istringstream_read);
BENCHMARK(seqan2_dna5_istringstream_read);
#endif // SEQAN3_HAS_SEQAN2

BENCHMARK_MAIN();
