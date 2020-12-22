#include <sstream>

#include <benchmark/benchmark.h>

#include <seqan3/nio/sequence_file/format/format_fasta.hpp>
#include <seqan3/nio/sequence_file/sequence_file_input.hpp>

#include "format_fasta_benchmark.hpp"

void seqan3_nio_char_read(benchmark::State & state)
{
    std::istringstream istream{fasta_file};

    size_t dummy{};

    for (auto _ : state)
    {
        istream.clear();
        istream.seekg(0, std::ios::beg);

        seqan3::nio::sequence_file_input fin{istream};
        seqan3::nio::format_fasta fasta{};
        fin.set_format(fasta);

        for (auto && record: fin)
        {
            dummy += record.sequence().size();
        }
    }

    size_t bytes_per_run = fasta_file.size();
    state.counters["iterations_per_run"] = iterations_per_run;
    state.counters["bytes_per_run"] = bytes_per_run;
    state.counters["bytes_per_second"] = seqan3::test::bytes_per_second(bytes_per_run);
    state.counters["dummy"] = dummy;
}
