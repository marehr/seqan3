// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2020, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2020, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#pragma once

#include <benchmark/benchmark.h>

#include <seqan3/test/seqan2.hpp>

#if SEQAN3_HAS_SEQAN2
void seqan2_dna5_ostringstream_write(benchmark::State & state);
#endif // SEQAN3_HAS_SEQAN2
