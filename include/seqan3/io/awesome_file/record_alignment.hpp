#pragma once

#include <type_traits>
#include <vector>

#include <seqan3/core/type_list/type_list.hpp>
#include <seqan3/io/awesome_file/record_base.hpp>
#include <seqan3/io/stream/iterator.hpp>
#include <seqan3/range/views/type_reduce.hpp>

namespace seqan3::awesome
{

// * | #  | SAM Column ID |  FIELD name                                       |
// * |:--:|:--------------|:--------------------------------------------------|
// * | 1  | QNAME         | seqan3::field::id                                 |
// * | 2  | FLAG          | seqan3::field::flag                               |
// * | 3  | RNAME         | seqan3::field::ref_id                             |
// * | 4  | POS           | seqan3::field::ref_offset                         |
// * | 5  | MAPQ          | seqan3::field::mapq                               |
// * | 6  | CIGAR         | implicilty stored in seqan3::field::alignment     |
// * | 7  | RNEXT         | seqan3::field::mate (tuple pos 0)                 |
// * | 8  | PNEXT         | seqan3::field::mate (tuple pos 1)                 |
// * | 9  | TLEN          | seqan3::field::mate (tuple pos 2)                 |
// * | 10 | SEQ           | seqan3::field::seq                                |
// * | 11 | QUAL          | seqan3::field::qual                               |

// Some basic stuff
enum struct field_
{
    qname,
    flag,
    rname,
    pos,
    mapq,
    cigar,
    rnext,
    pnext,
    tlen
    seq,
    qual
};

// template <typename ...policies_t>
class record_alignment
{
private:
    record_alignment * derived{};

protected:

    using return_t = decltype(std::declval<std::vector<char> &>() | views::type_reduce;
public:

    //!\brief A type list over the valid fields wrapped as std::integral_constant types.
    using valid_fields_type = field_id_type_list<field::id, field::seq, field::qual>;

    record_alignment() noexcept
    {
        derived = this;
    }
    record_alignment(record_alignment const &) = default;
    record_alignment(record_alignment &&) = default;
    record_alignment & operator=(record_alignment const &) = default;
    record_alignment & operator=(record_alignment &&) = default;
    virtual ~record_alignment() = default;

    // Contract interface: Needs to be called by the implementor.
    virtual void clear()
    {
        throw std::runtime_error{"Clear was not implemented."};
    }

    return_t qname()
    {
        assert(derived != nullptr);
        return derived->qname_impl();
    }

    return_t flag()
    {
        assert(derived != nullptr);
        return derived->flag_impl();
    }

    return_t rname()
    {
        assert(derived != nullptr);
        return derived->rname_impl();
    }

    return_t pos()
    {
        assert(derived != nullptr);
        return derived->pos_impl();
    }

    return_t mapq()
    {
        assert(derived != nullptr);
        return derived->mapq_impl();
    }

    return_t cigar()
    {
        assert(derived != nullptr);
        return derived->cigar_impl();
    }

    return_t rnext()
    {
        assert(derived != nullptr);
        return derived->rnext_impl();
    }

    return_t pnext()
    {
        assert(derived != nullptr);
        return derived->pnext_impl();
    }

    return_t tlen()
    {
        assert(derived != nullptr);
        return derived->tlen_impl();
    }

    return_t seq()
    {
        assert(derived != nullptr);
        return derived->seq_impl();
    }

    return_t qual()
    {
        assert(derived != nullptr);
        return derived->qual_impl();
    }

protected:

    virtual return_t qname_impl()
    {
        throw std::runtime_error{"The field was not set by the record."};
        return derived->qname_impl();
    }

    virtual return_t flag_impl()
    {
        throw std::runtime_error{"The field was not set by the record."};
        return derived->flag_impl();
    }

    virtual return_t rname_impl()
    {
        throw std::runtime_error{"The field was not set by the record."};
        return derived->rname_impl();
    }

    virtual return_t pos_impl()
    {
        throw std::runtime_error{"The field was not set by the record."};
        return derived->pos_impl();
    }

    virtual return_t mapq_impl()
    {
        throw std::runtime_error{"The field was not set by the record."};
        return derived->mapq_impl();
    }

    virtual return_t cigar_impl()
    {
        throw std::runtime_error{"The field was not set by the record."};
        return derived->cigar_impl();
    }

    virtual return_t rnext_impl()
    {
        throw std::runtime_error{"The field was not set by the record."};
        return derived->rnext_impl();
    }

    virtual return_t pnext_impl()
    {
        throw std::runtime_error{"The field was not set by the record."};
        return derived->pnext_impl();
    }

    virtual return_t tlen_impl()
    {
        throw std::runtime_error{"The field was not set by the record."};
        return derived->tlen_impl();
    }

    virtual return_t seq_impl()
    {
        throw std::runtime_error{"The field was not set by the record."};
        return derived->seq_impl();
    }

    virtual return_t qual_impl()
    {
        throw std::runtime_error{"The field was not set by the record."};
        return derived->qual_impl();
    }

    // Needs to be called by the derived class to register at the base class.
    void register_record(record_alignment * derived) noexcept
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

} // namespace seqan3::awesome
