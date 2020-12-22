#pragma once

#include <functional>
#include <type_traits>
#include <utility>

#include <seqan3/core/type_list/traits.hpp>

namespace seqan3::awesome
{

template <typename record_base_t, auto ...field_ids>
class record_get_adaptor
{
private:

    static constexpr size_t field_id_count = sizeof...(field_ids);

    template <size_t index>
    static constexpr auto field_id_at =
        seqan3::pack_traits::at<index, std::integral_constant<decltype(field_ids), field_ids>...>::value;

    template <auto field_id>
    struct compare
    {
        template <typename rhs_t>
        struct with
        {
            static constexpr bool value = (field_id == rhs_t::field);
        };
    };

    record_base_t const * base_record{};

public:

    record_get_adaptor() = default;
    record_get_adaptor(record_get_adaptor const &) = default;
    record_get_adaptor(record_get_adaptor &&) = default;
    record_get_adaptor & operator=(record_get_adaptor const &) = default;
    record_get_adaptor & operator=(record_get_adaptor &&) = default;
    ~record_get_adaptor() = default;

    record_get_adaptor(record_base_t const & base) : base_record{std::addressof(base)}
    {}

    template <size_t index>
        requires (index < field_id_count)
    auto get()
    {
        return get<field_id_at<index>>();
    }

    template <auto field_id>
        requires ((field_id == field_ids) || ...)
    auto get()
    {
        using map_t = typename record_base_t::field_to_function_map;
        constexpr size_t pos = seqan3::list_traits::find_if<compare<field_id>::template with, map_t>;

        static_assert(pos != -1, "What is pos?");

        using field_to_function_pair_t = seqan3::list_traits::at<pos, map_t>;
        return std::invoke(field_to_function_pair_t::function, *base_record);
    }
};
} // namespace seqan3::awesome

namespace std
{

template <typename record_base_t, auto ...field_ids>
struct tuple_size<seqan3::awesome::record_get_adaptor<record_base_t, field_ids...>> :
    public std::integral_constant<size_t, sizeof...(field_ids)>
{};

template <size_t index, typename record_base_t, auto ...field_ids>
struct tuple_element<index, seqan3::awesome::record_get_adaptor<record_base_t, field_ids...>>
{
    using decorator_t = seqan3::awesome::record_get_adaptor<record_base_t, field_ids...>;

    using type = std::remove_reference_t<decltype(std::declval<decorator_t>().template get<index>())>;
};

} // namespace std
