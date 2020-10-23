// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2020, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2020, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#include <algorithm>
#include <cctype>
#include <cstring>
#include <execution>

#include <benchmark/benchmark.h>

#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/alphabet/nucleotide/dna15.hpp>
#include <seqan3/io/awesome_file/decorated_file.hpp>
#include <seqan3/io/awesome_file/sequence_file_in.hpp>
#include <seqan3/io/awesome_file/format_fasta.hpp>
#include <seqan3/range/views/char_to.hpp>
#include <seqan3/range/views/convert.hpp>
#include <seqan3/range/views/to.hpp>
#include <seqan3/test/performance/units.hpp>

#include <sstream>

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

using char_buffer = std::vector<char>;

template <typename buf_it_t, typename del_t>
void skip_until(buf_it_t buf_it, del_t del)
{
    for (; buf_it != std::default_sentinel && !del(*buf_it); ++buf_it)
    {}
}

template <typename buf_it_t, typename delimiter_t>
void read_until(buf_it_t it, char_buffer & buffer, delimiter_t && delimiter)
{
    for (; it != std::default_sentinel && !delimiter(*it); ++it)
        buffer.push_back(*it);
}

template <typename buf_it_t>
void read_record(buf_it_t it, char_buffer & id, char_buffer & seq)
{
    id.clear();
    seq.clear();

    ++it;
    skip_until(it, seqan3::is_graph); // Until the first non space character.
    read_until(it, id, seqan3::is_char<'\n'> || seqan3::is_char<'\r'>);

    auto seq_field_delimiter = seqan3::is_char<'>'> || seqan3::is_eof;

    while (it != std::default_sentinel && !seq_field_delimiter(*it))
    {
        read_until(it, seq, seqan3::is_space);
        skip_until(it, seqan3::is_graph);
    }
}

void pure_copy(benchmark::State & state)
{
    std::istringstream istream{fasta_file};
    char_buffer id{};
    char_buffer seq{};

    for (auto _ : state)
    {
        istream.clear();
        istream.seekg(0, std::ios::beg);

        seqan3::detail::fast_istreambuf_iterator<char> it{*istream.rdbuf()};

        for (size_t j = 0; j < iterations_per_run; ++j)
            read_record(it, id, seq);
    }

    size_t bytes_per_run = fasta_file.size();
    state.counters["iterations_per_run"] = iterations_per_run;
    state.counters["bytes_per_run"] = bytes_per_run;
    state.counters["bytes_per_second"] = seqan3::test::bytes_per_second(bytes_per_run);
}
BENCHMARK(pure_copy);

// Ich hab die 2GB/s geknackt für fasta :) bytes_per_second=2.15673G/s (seqan2 hat 901.135M/s, altes seqan3 hat 493.039M/s)
// ich hab gerade noch eine technik aus seqan2 eingebaut und :
// "seqan3_nio_char_read" 1152 auf 1471
// seqan2 hat 1225
// seqan3 nur 721
// my optimised_copy 1455

// Make them behave nicely. -> Look into SeqAn2: Optimise with Marcel together.
// before: seqan3_nio_char_read.iterations 2087
// after: seqan3_nio_char_read.iterations 3033

struct parser
{
    using stream_buffer_t = seqan3::detail::stream_buffer_exposer<char>;
    stream_buffer_t * stream_buf{};
    bool eof{false};

    parser(std::istream & stream) : stream_buf{reinterpret_cast<stream_buffer_t *>(stream.rdbuf())}
    {
        assert(stream_buf != nullptr);
        eof = seqan3::is_eof(stream_buf->underflow()); // Make sure the buffer is filled.

    }

    void read_record(char_buffer & id, char_buffer & seq)
    {
        id.clear();
        seq.clear();

        assert(stream_buf != nullptr);
        // Skip the > tag
        assert(seqan3::is_char<'>'>(*stream_buf->gptr()));
        stream_buf->gbump(1);
        char * buffer_ptr = stream_buf->gptr();

        auto skip_until = [&](auto delimiter)
        {
            while (!eof && buffer_ptr != stream_buf->egptr() && !delimiter(*buffer_ptr))
            {
                if (buffer_ptr == stream_buf->egptr()) // Reached end of buffer but not the delimiter
                {
                    eof = seqan3::is_eof(stream_buf->underflow());  // We also need to check that the returned char is not eof.
                    buffer_ptr = stream_buf->gptr();
                    continue;
                }
                else
                {
                    ++buffer_ptr;
                }
            }
            stream_buf->gbump(buffer_ptr - stream_buf->gptr());
            eof |= (buffer_ptr == stream_buf->egptr());
        };

        // Skip until begin of id.
        skip_until(seqan3::is_graph);
        // Check we are at the begin of the id.
        assert(seqan3::is_graph(*buffer_ptr));
        assert(!eof);

        auto copy_to = [&] (auto & target, size_t const count)
        {
            // Save the first part of the id before underflow.
            // size_t const id_length = id_ptr - stream_buf->gptr();
            size_t const old_length = target.size();
            target.resize(old_length + count);
            std::memcpy(target.data() + old_length, stream_buf->gptr(), count);
            stream_buf->gbump(count); // Set the stream's gptr to the new size.
        };

        // Find end of id.
        auto is_newline = seqan3::is_char<'\r'> || seqan3::is_char<'\n'>;
        while (!eof && buffer_ptr != stream_buf->egptr() && !is_newline(*buffer_ptr))
        {
            if (buffer_ptr == stream_buf->egptr()) // Reached end of buffer but not begin of id.
            {
                copy_to(id, buffer_ptr - stream_buf->gptr());
                eof = seqan3::is_eof(stream_buf->underflow());  // We also need to check that the returned char is not eof.
                buffer_ptr = stream_buf->gptr();
                continue; // continue with new buffered elements.
            }
            else
            {
                ++buffer_ptr;
            }
        }

        // Not expecting to be at end of the stream here.
        assert(!eof);
        // Found end of id: So we add the buffer to it.
        copy_to(id, buffer_ptr - stream_buf->gptr());

        // ----------------------------------------------------------------------------
        // read seq!
        // ----------------------------------------------------------------------------

        skip_until(seqan3::is_alpha); // move the buffer_ptr to the next char
        assert(!eof);

        // Loop until we are at the begin of the next record or at the eof stream.
        while (!eof && !seqan3::is_char<'>'>(*buffer_ptr))
        {
            while (!eof && buffer_ptr != stream_buf->egptr() && !seqan3::is_space(*buffer_ptr))
            {
                if (buffer_ptr == stream_buf->egptr()) // Reached end of buffer but not begin of id.
                {
                    copy_to(seq, buffer_ptr - stream_buf->gptr());
                    eof = seqan3::is_eof(stream_buf->underflow());  // We also need to check that the returned char is not eof.
                    buffer_ptr = stream_buf->gptr();
                    continue; // continue with new buffered elements.
                }
                else
                {
                    ++buffer_ptr;
                }
            }
            copy_to(seq, buffer_ptr - stream_buf->gptr());
            skip_until(seqan3::is_graph);
        }
    }
};

void optimised_copy(benchmark::State & state)
{
    std::istringstream istream{fasta_file};
    char_buffer id{};
    char_buffer seq{};

    for (auto _ : state)
    {
        istream.clear();
        istream.seekg(0, std::ios::beg);

        parser p{istream};

        for (size_t j = 0; j < iterations_per_run; ++j)
        {
            p.read_record(id, seq);
        }
    }

    size_t bytes_per_run = fasta_file.size();
    state.counters["iterations_per_run"] = iterations_per_run;
    state.counters["bytes_per_run"] = bytes_per_run;
    state.counters["bytes_per_second"] = seqan3::test::bytes_per_second(bytes_per_run);
}
BENCHMARK(optimised_copy);

void from_fasta(benchmark::State & state)
{
    std::istringstream istream{fasta_file};
    seqan3::awesome::format_fasta fmt{};

    for (auto _ : state)
    {
        istream.clear();
        istream.seekg(0, std::ios::beg);

        // seqan3::detail::fast_istreambuf_iterator<char> it{*istream.rdbuf()};

        for (size_t j = 0; j < iterations_per_run; ++j)
            fmt.read_record(istream);
    }

    size_t bytes_per_run = fasta_file.size();
    state.counters["iterations_per_run"] = iterations_per_run;
    state.counters["bytes_per_run"] = bytes_per_run;
    state.counters["bytes_per_second"] = seqan3::test::bytes_per_second(bytes_per_run);
}
BENCHMARK(from_fasta);

void from_format_base(benchmark::State & state)
{
    std::istringstream istream{fasta_file};
    using fmt_base_t = seqan3::awesome::format_base<seqan3::awesome::record_sequence>;
    std::unique_ptr<fmt_base_t> fmt{std::make_unique<seqan3::awesome::format_fasta<>>()};

    for (auto _ : state)
    {
        istream.clear();
        istream.seekg(0, std::ios::beg);

        for (size_t j = 0; j < iterations_per_run; ++j)
            fmt->read_record(istream);
    }

    size_t bytes_per_run = fasta_file.size();
    state.counters["iterations_per_run"] = iterations_per_run;
    state.counters["bytes_per_run"] = bytes_per_run;
    state.counters["bytes_per_second"] = seqan3::test::bytes_per_second(bytes_per_run);
}
BENCHMARK(from_format_base);

void read_awesome(benchmark::State & state)
{
    std::istringstream istream{fasta_file};
    seqan3::awesome::sequence_file_in fin{istream, seqan3::awesome::format_fasta{}};

    for (auto _ : state)
    {
        istream.clear();
        istream.seekg(0, std::ios::beg);

        auto it = fin.begin();
        for (size_t j = 0; j < iterations_per_run; ++j)
            it++;
    }

    size_t bytes_per_run = fasta_file.size();
    state.counters["iterations_per_run"] = iterations_per_run;
    state.counters["bytes_per_run"] = bytes_per_run;
    state.counters["bytes_per_second"] = seqan3::test::bytes_per_second(bytes_per_run);
}

BENCHMARK(read_awesome);

void read_awesome_decorated(benchmark::State & state)
{
    std::istringstream istream{fasta_file};

    using file_t = seqan3::awesome::decorated_file<seqan3::awesome::sequence_file_in,
                                                   seqan3::awesome::field::seq>;
    file_t fin{seqan3::awesome::sequence_file_in{istream, seqan3::awesome::format_fasta{}}};

    for (auto _ : state)
    {
        istream.clear();
        istream.seekg(0, std::ios::beg);

        auto it = fin.begin();
        for (size_t j = 0; j < iterations_per_run; ++j)
        {
            auto && [seq] = *it;
            std::ranges::distance(seq | seqan3::views::char_to<seqan3::dna15> | seqan3::views::convert<seqan3::dna5>);
            it++;
        }
    }

    size_t bytes_per_run = fasta_file.size();
    state.counters["iterations_per_run"] = iterations_per_run;
    state.counters["bytes_per_run"] = bytes_per_run;
    state.counters["bytes_per_second"] = seqan3::test::bytes_per_second(bytes_per_run);
}

BENCHMARK(read_awesome_decorated);

void read_awesome_with_conversion(benchmark::State & state)
{
    std::istringstream istream{fasta_file};
    seqan3::awesome::sequence_file_in fin{istream, seqan3::awesome::format_fasta{}};
    size_t count = 0;
    for (auto _ : state)
    {
        istream.clear();
        istream.seekg(0, std::ios::beg);

        auto it = fin.begin();
        for (size_t j = 0; j < iterations_per_run; ++j)
        {
            // seq.clear();
            // size_t s = std::ranges::distance(it->seq());
            // seq.resize(s);
            count = std::ranges::count(it->seq() | seqan3::views::char_to<seqan3::dna5>/* | seqan3::views::convert<seqan3::dna5>*/,
                                       seqan3::assign_rank_to(0, seqan3::dna5{}));
            // std::copy_n(std::execution::par, dna5_view.begin(), s, seq.begin());
            it++;
        }
    }

    size_t bytes_per_run = fasta_file.size();
    state.counters["iterations_per_run"] = iterations_per_run;
    state.counters["bytes_per_run"] = bytes_per_run;
    state.counters["bytes_per_second"] = seqan3::test::bytes_per_second(bytes_per_run);
    state.counters["count"] = count;
}

BENCHMARK(read_awesome_with_conversion);

BENCHMARK_MAIN();
