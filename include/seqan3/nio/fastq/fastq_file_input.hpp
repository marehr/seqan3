// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \brief Provides .
 * \author Rene Rahn <rene.rahn AT fu-berlin.de>
 */

#pragma once

#include <seqan3/std/filesystem>
#include <fstream>
#include <iostream>
#include <seqan3/std/iterator>
#include <memory>
#include <seqan3/std/ranges>
#include <string>

// #include <seqan3/nio/fastq/format.hpp>
// #include <seqan3/nio/fastq/raw_record.hpp>

struct fastq_record_raw
{
    std::string _id_raw;
    std::string _sequence_raw;
    std::string _quality_raw;

    std::string id_raw() { return _id_raw; }
    std::string sequence_raw() { return _sequence_raw; }
    std::string quality_raw() { return _quality_raw; }
};

class fastq_format
{
public:

    fastq_record_raw read_record(std::istream & /*istream*/)
    {
        return {"id", "seq", "qual"};
    }
};

namespace seqan3::nio
{

class fastq_file_input
{
private:
    using record_t = fastq_record_raw;

    class iterator
    {
    private:

        fastq_file_input * host{nullptr};
    public:

        using value_type = record_t;
        using reference = std::add_lvalue_reference_t<value_type>;
        using pointer = std::add_pointer_t<value_type>;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        iterator() = default;
        explicit iterator(fastq_file_input * file) : host{file}
        {
            ++(*this); // Read first record on construction.
        }

        reference operator*() const
        {
            assert(host != nullptr);
            return host->cached_record;
        }

        pointer operator->() const
        {
            assert(host != nullptr);
            return std::addressof(host->cached_record);
        }

        iterator & operator++()
        {
            assert(host != nullptr);
            host->cached_record = host->format.read_record(*(host->base_istream));
            return *this;
        }

        void operator++(int)
        {
            ++*this;
        }

        bool operator==(std::default_sentinel_t const &) const noexcept
        {
            assert(host != nullptr);
            return std::istreambuf_iterator<char>{*(host->base_istream)} == std::istreambuf_iterator<char>{};
        }
    };

    record_t cached_record{};
    fastq_format format{};
    std::unique_ptr<std::istream> base_istream{nullptr};
public:

    fastq_file_input() = default;

    fastq_file_input(std::filesystem::path const & path)
    {
        base_istream = std::make_unique<std::ifstream>(path.c_str());
    }

    iterator begin()
    {
        return iterator{this};
    }

    std::default_sentinel_t end()
    {
        return std::default_sentinel;
    }
};

}  // namespace seqan3::nio
