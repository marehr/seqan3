#pragma once

#include <type_traits>
#include <vector>

#include <seqan3/core/type_list/type_list.hpp>
#include <seqan3/io/stream/iterator.hpp>
#include <seqan3/range/views/slice.hpp>

namespace seqan3::awesome
{
// Some basic stuff
enum struct field
{
    id,
    seq,
    qual
};

// Transforms a list of field ids into a type list of integral constants storing the field ids.
template <auto ...field_ids>
using field_id_type_list = seqan3::type_list<std::integral_constant<decltype(field_ids), field_ids>...>;

// template <typename ...policies_t>
class sequence_record
{
private:
    sequence_record * derived{};

protected:

    using return_t = decltype(std::declval<std::vector<char> &>() | views::slice(0, 0));
public:

    //!\brief A type list over the valid fields wrapped as std::integral_constant types.
    using valid_fields_type = field_id_type_list<field::id, field::seq, field::qual>;

    sequence_record() noexcept
    {
        derived = this;
    }
    sequence_record(sequence_record const &) = default;
    sequence_record(sequence_record &&) = default;
    sequence_record & operator=(sequence_record const &) = default;
    sequence_record & operator=(sequence_record &&) = default;
    virtual ~sequence_record() = default;

    // Contract interface: Needs to be called by the implementor.
    virtual void clear()
    {
        throw std::runtime_error{"Clear was not implemented."};
    }

    return_t id() const
    {
        assert(derived != nullptr);
        return derived->id_impl();
    }

    return_t seq() const
    {
        assert(derived != nullptr);
        return derived->seq_impl();
    }

    return_t qual() const
    {
        assert(derived != nullptr);
        return derived->qual_impl();
    }

protected:
    virtual return_t id_impl()
    {
        handle_base_error();
        throw std::runtime_error{"The id was not set by the format."};
        return return_t{};
    }

    virtual return_t seq_impl()
    {
        handle_base_error();
        throw std::runtime_error{"The seq was not set by the format."};
        return return_t{};
    }

    virtual return_t qual_impl()
    {
        handle_base_error();
        throw std::runtime_error{"The qual was not set by the format."};
        return return_t{};
    }

    // Needs to be called by the derived class to register at the base class.
    void register_record(sequence_record * derived) noexcept
    {
        if (derived == this)
            std::runtime_error{"You must register a derived class of this record."};

        this->derived = derived;
    }

private:

    void handle_base_error()
    {
        if (this == derived)
            throw std::runtime_error{"The record implementation was not set. Did you forget to call register_record?"};
    }
};

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
