#pragma once

#include <memory>

#include <seqan3/core/platform.hpp>
#include <seqan3/offload/mpi_backend.hpp>

namespace seqan3::detail
{

template<typename deleter_t, typename value_t>
struct delete_buffer {
    using result_type = void;

    result_type operator()()
    {
        assert(ham::offload::this_node() == ptr.node());
        deleter(ptr.get());
    }

    ham::net::buffer_ptr<value_t> ptr{};
    deleter_t deleter{};
};

} // namespace seqan3::detail

namespace seqan3::offload
{

template <typename value_t>
struct free_delete
{
    void operator()(value_t* ptr) const
    {
        free(ptr);
    }
};

template <typename value_t>
struct no_delete
{
    void operator()(value_t*) const {}
};

template<typename value_t>
using default_delete = std::default_delete<value_t>;

template <typename value_t, typename deleter_t>
void free(ham::offload::buffer_ptr<value_t> buffer, deleter_t deleter)
{
    seqan3::detail::delete_buffer<deleter_t, value_t> deleter_fn
    {
        buffer,
        std::forward<deleter_t>(deleter)
    };
    ham::offload::sync(buffer.node(), deleter_fn);
}

} // namespace seqan3::offload
