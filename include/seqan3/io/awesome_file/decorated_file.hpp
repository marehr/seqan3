#pragma once

#include <seqan3/std/ranges>

#include <seqan3/io/awesome_file/file_base.hpp>
#include <seqan3/io/awesome_file/get_decorated_record.hpp>

namespace seqan3::awesome
{

template <typename file_t, auto ...field_ids>
class decorated_file
{
private:

    using record_type = std::ranges::range_value_t<file_t>;
    using decorated_record_type = get_decorated_record<record_type, field_ids...>;

    static constexpr auto decorate_record = [] (record_type & rec) -> decltype(decorated_record_type{rec})
    {
        return decorated_record_type{rec};
    };

    static constexpr auto file_adaptor = std::views::transform(decorate_record);

    using decorated_file_type = decltype(std::declval<file_t &>() | file_adaptor);

    file_t original_file{};
    decorated_file_type file{};

public:

    decorated_file() = default;
    decorated_file(decorated_file const &) = delete;
    decorated_file(decorated_file &&) = default;
    decorated_file & operator=(decorated_file const &) = delete;
    decorated_file & operator=(decorated_file &&) = default;
    ~decorated_file() = default;

    explicit decorated_file(file_t && original_file) : original_file{std::move(original_file)}
    {
        file = this->original_file | file_adaptor;
    }

    auto begin()
    {
        return file.begin();
    }

    auto begin() const = delete;

    auto end()
    {
        return file.end();
    }

    auto end() const = delete;
};

} // namespace seqan3::awesome
