#pragma once

#include <type_traits>
#include <vector>

#include <seqan3/core/type_list/type_list.hpp>
#include <seqan3/io/awesome_file/record_base.hpp>
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

// template <typename ...policies_t>
class record_sequence
{
private:
    record_sequence * derived{};

protected:

    using return_t = decltype(std::declval<std::vector<char> &>() | views::slice(0, 0));
public:

    //!\brief A type list over the valid fields wrapped as std::integral_constant types.
    using valid_fields_type = field_id_type_list<field::id, field::seq, field::qual>;

    record_sequence() noexcept
    {
        derived = this;
    }
    record_sequence(record_sequence const &) = default;
    record_sequence(record_sequence &&) = default;
    record_sequence & operator=(record_sequence const &) = default;
    record_sequence & operator=(record_sequence &&) = default;
    virtual ~record_sequence() = default;

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

    using field_to_function_map = seqan3::type_list<field_to_function<field::id, &record_sequence::id>,
                                                    field_to_function<field::seq, &record_sequence::seq>,
                                                    field_to_function<field::qual, &record_sequence::qual>>;

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
    void register_record(record_sequence * derived) noexcept
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

template <typename derived_t, typename record_base_t>
    requires std::derived_from<record_base_t, record_sequence>
class record_sequence_wrapper : public record_base_t
{
private:
    friend derived_t;

    using typename record_base_t::return_t;

    record_sequence_wrapper() = default;
    record_sequence_wrapper(record_sequence_wrapper const &) = default;
    record_sequence_wrapper(record_sequence_wrapper &&) = default;
    record_sequence_wrapper & operator=(record_sequence_wrapper const &) = default;
    record_sequence_wrapper & operator=(record_sequence_wrapper &&) = default;
    ~record_sequence_wrapper() = default;

    return_t id_impl() override
    {
        return as_derived()->id();
    }

    return_t seq_impl() override
    {
        return as_derived()->seq();
    }

    return_t qual_impl() override
    {
        return as_derived()->qual();
    }

    derived_t * as_derived()
    {
        return static_cast<derived_t *>(this);
    }
};

} // namespace seqan3::awesome
