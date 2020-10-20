#pragma once

#include <utility>

#include <seqan3/core/type_list/traits.hpp>

namespace seqan3::awesome
{

template <typename record_base_t, auto ...field_ids>
class get_decorated_record : public record_base_t
{
private:

    static constexpr size_t field_id_count = sizeof...(field_ids);

    template <auto index>
    using field_id_at = seqan3::pack_traits::at<index, std::integral_constant<decltype(field_ids), field_ids>...>;

public:

    get_decorated_record() = default;
    get_decorated_record(get_decorated_record const &) = default;
    get_decorated_record(get_decorated_record &&) = default;
    get_decorated_record & operator=(get_decorated_record const &) = default;
    get_decorated_record & operator=(get_decorated_record &&) = default;
    ~get_decorated_record() = default;

    get_decorated_record(record_base_t const & base) : record_base_t{base}
    {}

    using record_base_t::record_base_t;

    template <size_t index>
        requires (index < field_id_count)
    auto get()
    {
        return record_base_t::get_field(field_id_at<index>::value);
    }
};
} // namespace seqan3::awesome

namespace std
{

template <typename record_base_t, auto ...field_ids>
struct tuple_size<seqan3::awesome::get_decorated_record<record_base_t, field_ids...>> :
    public std::integral_constant<size_t, sizeof...(field_ids)>
{};

template <size_t index, typename record_base_t, auto ...field_ids>
struct tuple_element<index, seqan3::awesome::get_decorated_record<record_base_t, field_ids...>>
{
    using decorator_t = seqan3::awesome::get_decorated_record<record_base_t, field_ids...>;

    using type = std::remove_reference_t<decltype(std::declval<decorator_t>().template get<index>())>;
};

} // namespace std
