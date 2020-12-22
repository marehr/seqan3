// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2020, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2020, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#pragma once

#include <string>

#include <seqan3/test/performance/units.hpp>

inline constexpr size_t iterations_per_run = 1024;

inline std::string const fasta_hdr{"seq foobar blobber"};
inline std::string const fasta_seq{
    "ACTAGACTAGCTACGATCAGCTACGATCAGCTACGAACTAGACTAGCTACGATACTAGACTAGCTACGATCAGCTACGA"
    "ACTAGACTAGCTACGATCAGCTACGATCAGCTACGAACTAGACTAGCTACGATACTAGACTAGCTACGATCAGCTACGA"
    "ACTAGACTAGCTACGATCAGCTACGATCAGCTACGAACTAGACTAGCTACGATACTAGACTAGCTACGATCAGCTACGA"
    "ACTAGACTAGCTACGATCAGCTACGATCAGCTACGAACTAGACTAGCTACGATACTAGACTAGCTACGATCAGCTACGA"
    "ACTAGACTAGCTACGATCAGCTACGATCAGCTACGAACTAGACTAGCTACGATACTAGACTAGCTACGATCAGCTACGA"
    "ACTAGACTAGCTACGATCAGCTACGATCAGCTACGAACTAGACTAGCTACGATACTAGACTAGCTACGATCAGCTACGA"};
//TODO: benchmark with spaces/newlines

static std::string fasta_file = []()
{
    std::string file{};
    for (size_t idx = 0; idx < iterations_per_run; idx++)
        file += "> " + fasta_hdr + "\n" + fasta_seq + "\n";
    return file;
}();
