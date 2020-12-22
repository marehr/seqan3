// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \brief Provides seqan3::formated_file_input.
 * \author Rene Rahn <rene.rahn AT fu-berlin.de>
 */

#pragma once

#include <seqan3/std/algorithm>
#include <seqan3/std/filesystem>
#include <fstream>
#include <iostream>
#include <seqan3/std/iterator>
#include <memory>
#include <seqan3/std/ranges>
#include <string>
#include <seqan3/std/type_traits>

#include <seqan3/core/detail/pack_algorithm.hpp>

namespace seqan3
{

template <typename record_t>
class formatted_file_input
{
private:
    using stream_ptr_t = std::unique_ptr<std::basic_istream<char>,
                                         std::function<void(std::basic_istream<char>*)>>;
    //!\brief Stream deleter that does nothing (no ownership assumed).
    static void non_owning_deleter(std::basic_istream<char> *) {}
    //!\brief Stream deleter with default behaviour (ownership assumed).
    static void owning_deleter(std::basic_istream<char> * ptr) { delete ptr; }
public:
    class iterator;

    /*!\name Constructors, destructor and assignment
     * \{
     */
    constexpr formatted_file_input() = default; //!< Defaulted.
    constexpr formatted_file_input(formatted_file_input const &) = default; //!< Defaulted.
    constexpr formatted_file_input(formatted_file_input &&) = default; //!< Defaulted.
    constexpr formatted_file_input & operator=(formatted_file_input const &) = default; //!< Defaulted.
    constexpr formatted_file_input & operator=(formatted_file_input &&) = default; //!< Defaulted.
    ~formatted_file_input() = default; //!< Defaulted.

    template <typename ...formats_t>
        requires (sizeof...(formats_t) > 0)
    explicit constexpr formatted_file_input(std::filesystem::path path, formats_t && ...formats)
    {
        // Read the extension and guess the format.
        std::string file_ext = path.extension().string().substr(1);

        seqan3::detail::for_each([&] (auto format)
        {
            // The first format that is found will be set.
            if (!selected_format &&
                std::ranges::any_of(format.extensions(), [&] (std::string ext) { return ext == file_ext; }))
            {
                using format_t = decltype(format);
                selected_format = std::make_unique<any_format<format_t>>(std::move(format));
            }
        }, std::forward<formats_t>(formats)...);

        // Next step: open file_handle from the path.
        // Also add the decompression format.
        // safe to call new here directly?
        base_istream = stream_ptr_t{new std::ifstream{path.c_str()}, owning_deleter};

        if (!base_istream->good())
            throw std::runtime_error{"Could not open the file stream."};
    }
    //!}

    iterator begin()
    {
        return iterator{this};
    }

    iterator begin() const = delete;


    std::default_sentinel_t end()
    {
        return std::default_sentinel;
    }

    std::default_sentinel_t end() const = delete;


private:

    void read_record()
    {
        assert(selected_format != nullptr);
        assert(base_istream != nullptr);

        if (std::istreambuf_iterator<char>{*base_istream} == std::istreambuf_iterator<char>{})
            is_at_end = true;
        else
            cached_record = selected_format->read_record(*base_istream);
    }

    struct input_format_concept
    {
        virtual ~input_format_concept() = default;
        virtual record_t read_record(std::istream &) = 0;
    };

    template <typename format_t>
    struct any_format final : public input_format_concept
    {

        any_format(format_t format) : format{std::move(format)}
        {}

        record_t read_record(std::istream & input_stream) override
        {
            return record_t{format.read_record(input_stream)};
        }

        format_t format{};
    };

    record_t cached_record{};
    std::unique_ptr<input_format_concept> selected_format{nullptr};
    stream_ptr_t base_istream{nullptr, non_owning_deleter};
    bool is_at_end{false};
};

template <typename record_t>
class formatted_file_input<record_t>::iterator
{
private:

    formatted_file_input * host{nullptr};
public:

    using value_type = record_t;
    using reference = std::add_lvalue_reference_t<value_type>;
    using pointer = std::add_pointer_t<value_type>;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    iterator() = default;
    explicit iterator(formatted_file_input * file) : host{file}
    {
        ++(*this);
    }

    reference operator*() const
    {
        assert(host != nullptr);
        // This can't be a header and a record at the same time? Can it?
        // It could by making it a variant, but do we want to?
        return host->cached_record;
    }

    pointer operator->() const
    {
        assert(host != nullptr);
        // This can't be a header and a record at the same time? Can it?
        // It could by making it a variant, but do we want to?
        return std::addressof(host->cached_record);
    }

    iterator & operator++()
    {
        assert(host != nullptr);
        host->read_record();
        return *this;
    }

    void operator++(int)
    {
        ++*this;
    }

    bool operator==(std::default_sentinel_t const &) const noexcept
    {
        assert(host != nullptr);
        return host->is_at_end;
    }
};
}  // namespace seqan3
