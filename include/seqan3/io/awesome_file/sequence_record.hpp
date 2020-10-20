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
    // add more
};

// Transforms a list of field ids into a type list of integral constants storing the field ids.
template <auto ...field_ids>
using field_id_type_list = seqan3::type_list<std::integral_constant<decltype(field_ids), field_ids>...>;

// template <typename ...policies_t>
class sequence_record
{
private:

    // This is potentially handled inside of a policy.
    // using buffer_interval_type = std::pair<size_t, size_t>;

    // // Might have a header pointer.
    // std::vector<char> buffer{};
    // std::vector<buffer_interval_type> buffer_field_positions{3 , {0, 0}};

    sequence_record * derived{};

protected:

    // auto get_field(field const field_id)
    // {
    //     int32_t field_pos = static_cast<int32_t>(field_id);
    //     return buffer | seqan3::views::slice(buffer_field_positions[field_pos].first,
    //                                          buffer_field_positions[field_pos].second);
    // }

    // auto get_field(field const field_id) const
    // {
    //     int32_t field_pos = static_cast<int32_t>(field_id);
    //     return buffer | seqan3::views::slice(buffer_field_positions[field_pos].first,
    //                                          buffer_field_positions[field_pos].second);
    // }

    using return_t = decltype(std::declval<std::vector<char> &>() | views::slice(0, 0));


    // std::vector<char> empty{};
public:

    //!\brief A type list over the valid fields wrapped as std::integral_constant types.
    using valid_fields_type = field_id_type_list<field::id, field::seq, field::qual>;

    sequence_record() = default;
    sequence_record(sequence_record const &) = default;
    sequence_record(sequence_record &&) = default;
    sequence_record & operator=(sequence_record const &) = default;
    sequence_record & operator=(sequence_record &&) = default;
    virtual ~sequence_record() = default;

    sequence_record(sequence_record * derived) : derived{derived}
    {}

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
        throw std::runtime_error{"The id was not set by the format."};
        return return_t{};
    }

    virtual return_t seq_impl()
    {
        throw std::runtime_error{"The seq was not set by the format."};
        return return_t{};
    }

    virtual return_t qual_impl()
    {
        throw std::runtime_error{"The qual was not set by the format."};
        return return_t{};
    }
};

} // namespace seqan3::awesome
