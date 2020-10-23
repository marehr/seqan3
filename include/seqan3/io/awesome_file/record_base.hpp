#pragma once

#include <type_traits>

#include <seqan3/core/type_list/type_list.hpp>

namespace seqan3::awesome
{
// Transforms a list of field ids into a type list of integral constants storing the field ids.
template <auto ...field_ids>
using field_id_type_list = seqan3::type_list<std::integral_constant<decltype(field_ids), field_ids>...>;


template <typename derived_record_t>
class record_registration_policy
{
private:
    friend derived_record_t;

    record_registration_policy() noexcept
    {
        register_record_for_derived();
    }

    record_registration_policy(record_registration_policy const &) noexcept : record_registration_policy{}
    {}

    record_registration_policy(record_registration_policy &&) noexcept : record_registration_policy{}
    {}

    record_registration_policy & operator=(record_registration_policy) noexcept
    {
        register_record_for_derived();
        return *this;
    }

    void register_record_for_derived() noexcept
    {
        as_derived()->register_record(as_derived());
    }

    derived_record_t * as_derived() noexcept
    {
        return static_cast<derived_record_t *>(this);
    }
};

} // namespace seqan3::awesome
