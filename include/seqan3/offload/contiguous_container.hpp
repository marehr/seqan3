// ==========================================================================
//                 SeqAn - The Library for Sequence Analysis
// ==========================================================================
//
// Copyright (c) 2006-2017, Knut Reinert, FU Berlin
// Copyright (c) 2016-2017, Knut Reinert & MPI Molekulare Genetik
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Knut Reinert or the FU Berlin nor the names of
//       its contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL KNUT REINERT OR THE FU BERLIN BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// ==========================================================================

#pragma once

#include <cassert>
#include <memory>

#include <seqan3/core/platform.hpp>
#include <seqan3/offload/target_migratable.hpp>
#include <seqan3/offload/sized_buffer_ptr.hpp>

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

template <typename value_t, typename deleter_t = free_delete<value_t>>
struct contiguous_container
{
    using pointer = value_t*;
    using value_type = value_t;
    using deleter_type = deleter_t;
    using unique_ptr_t = std::unique_ptr<value_t, deleter_t>;

    contiguous_container()
    = default;
    //     : _size{0u}, _data{nullptr}
    // {
    //     std::cout << "contiguous_container(): b: " << _data.get() << ", e:" << (_data.get() + _size) << ", " << _size << std::endl;
    // }
    contiguous_container(const contiguous_container&) = delete;
    contiguous_container(contiguous_container&& o) noexcept
    // = default;
        : _size{std::exchange(o._size, 0u)},
          _data{std::move(o._data)}
    {
        // std::swap(_size, o._size);
        // std::swap(_data, o._data);

        // std::cout << "contiguous_container(contiguous_container&& o): b: " << _data.get() << ", e:" << (_data.get() + _size) << ", " << _size << std::endl;
    }
    contiguous_container& operator=(const contiguous_container&) = delete;
    contiguous_container& operator=(contiguous_container&& o) noexcept
    // = default;
    {
        _size = std::exchange(o._size, 0u);
        _data = std::move(o._data);
        return *this;
    }
    // = default;
    // {
    //     std::swap(_size, o._size);
    //     std::swap(_data, o._data);
    //     std::cout << "operator=(contiguous_container&& o): b: " << _data.get() << ", e:" << (_data.get() + _size) << ", " << _size << std::endl;
    // };

    contiguous_container(size_t size, value_t* ptr)
        : _size{size}, _data{ptr}
    {
        // std::cout << "contiguous_container(size_t size, value_t* ptr): b: " << _data.get() << ", e:" << (_data.get() + _size) << ", " << _size << std::endl;
    }

    contiguous_container(size_t size)
        : contiguous_container(size, (value_t*)std::malloc(sizeof(value_t) * size))
    {
        // std::cout << "contiguous_container(size_t size): b: " << _data.get() << ", e:" << (_data.get() + _size) << ", " << _size << std::endl;
    }

    ~contiguous_container() = default;
    // {
    //     // std::cout << "~contiguous_container(): b: " << _data.get() << ", e:" << (_data.get() + _size) << ", " << _size << std::endl;
    // }

    pointer begin() const noexcept
    {
        // std::cout << "contiguous_container::begin(): b: " << _data.get() << ", e:" << (_data.get() + _size) << ", " << _size << std::endl;
        return _data.get();
    }
    pointer end() const noexcept
    {
        // std::cout << "contiguous_container::end(): b: " << _data.get() << ", e:" << (_data.get() + _size) << ", " << _size << std::endl;
        return _data.get() + _size;
    }
    size_t size() const noexcept
    {
        // std::cout << "contiguous_container::size(): b: " << _data.get() << ", e:" << (_data.get() + _size) << ", " << _size << std::endl;
        return _size;
    }
    pointer release() noexcept
    {
        _size = 0u;
        return _data.release();
    }
protected:
    size_t _size{0u};
    unique_ptr_t _data{nullptr};
};

template <typename contiguous_container_t, typename = void>
struct is_contiguous_container : std::false_type
{};

template <typename value_t, typename deleter_t>
struct is_contiguous_container<contiguous_container<value_t, deleter_t>, void> : std::true_type
{};

template <typename contiguous_container_t>
struct is_contiguous_container<contiguous_container_t, std::enable_if_t<
    !std::is_same_v<contiguous_container_t, std::decay_t<contiguous_container_t>>
>> : is_contiguous_container<std::decay_t<contiguous_container_t>>
{};

enum struct target_migratable_mode
{
    push_data,
    pull_data
};

// TODO: destructor
template<typename contiguous_container_t>
struct target_migratable<contiguous_container_t, std::enable_if_t<is_contiguous_container<contiguous_container_t>::value>>
{
    using value_t = typename std::decay_t<contiguous_container_t>::value_type;
    using deleter_t = typename std::decay_t<contiguous_container_t>::deleter_type;

    target_migratable() = default;
    target_migratable(const target_migratable&) = default;
    target_migratable(target_migratable&&) = default;
    target_migratable& operator=(const target_migratable&) = default;
    target_migratable& operator=(target_migratable&&) = default;

    target_migratable(node_t target_node, contiguous_container<value_t, deleter_t> const & contiguous_container)
        : _sized_buffer_ptr{}, _mode{target_node == node_t{0u} ? target_migratable_mode::pull_data : target_migratable_mode::push_data}
    {
        // NOTE: We have two cases where migrations are used to transfer data
        // when invoking a seqan3::offload::target_function
        //      vector<int> f(vector<int>, vector<int>)
        // 1) Each argument of a function `f` will be transferred from the host
        //    through a migratable to the target_node. HAM uses the classic
        //    heterogeneous model where the host issues all data-transfer and
        //    remote method invocations. That means the constructor is
        //    responsible for the data-transfer from host to the target_node,
        //    since this is the only function that will called on the host.
        //
        //    The constructor will not be called on the target_node, because
        //    the instance of this class will be copied by MPI to the
        //    target_node.
        // 2) The return type will be transferred... TODO

        // target_migratable will only be constructed on the node where the
        // data lives.
        assert(ham::offload::this_node() != target_node);
        // std::cout << "target_migratable<const contiguous_container&>-ctor&& on " << ham::offload::this_node() << " -> " << target_node << ": "; detail::print(contiguous_container); std::cout << std::endl;
        // host-to-node transfer
        if (mode(target_node) == target_migratable_mode::push_data)
        {
            // std::cout << ham::offload::this_node() << ": contiguous_container{" << target_node << "}: push_data" << std::endl;
            allocate_and_transfer_data_to_node(target_node, contiguous_container);
        }
        else
        {
            // std::cout << ham::offload::this_node() << ": contiguous_container{" << target_node << "}: pull_data" << std::endl;
            // uhhh; this is more complicated
            // in pull_data:
            // * constructor contiguous_container && (data only lives in remote call; must live longer)
            // * size = contiguous_container.size()
            // * raw_ptr = contiguous_container.release()
            // * _sized_buffer_ptr = {target_node, raw_ptr, size};
            //
            // Memory will be cleared in de-serialisation / destructor
            size_t size = contiguous_container.size();
            // std::cout << "\tcontiguous_container: " << contiguous_container.size() << ", " << contiguous_container.begin() << std::endl;
            value_t* raw_ptr = const_cast<seqan3::offload::contiguous_container<value_t, deleter_t>&>(contiguous_container).release();
            // std::cout << "\tcontiguous_container: " << contiguous_container.size() << ", " << contiguous_container.begin() << std::endl;
            _sized_buffer_ptr = {{raw_ptr, ham::offload::this_node()}, size};
            // std::cout << "\t_sized_buffer_ptr: " << "{{" << raw_ptr << ", " << ham::offload::this_node() << "}," << size << "}" << std::endl;
        }
    }

    target_migratable_mode mode([[maybe_unused]] node_t target_node) const
    {
        // return target_node == node_t{0u} ? target_migratable_mode::pull_data : target_migratable_mode::push_data;
        return _mode;
    }

    void allocate_and_transfer_data_to_node(node_t target_node, contiguous_container<value_t, deleter_t> const & contiguous_container)
    {
        size_t size = contiguous_container.size();
        _sized_buffer_ptr = {ham::offload::allocate<value_t>(target_node, size), size};
        ham::offload::put_sync(contiguous_container.begin(), _sized_buffer_ptr, size);
    }

    value_t* allocate_and_transfer_data_from_node(sized_buffer_ptr<value_t> & buffer) const
    {
        // std::cout << ":: " << "allocate_and_transfer_data_from_node" << std::endl;
        value_t* value_ptr = static_cast<value_t*>(std::malloc(buffer.size() * sizeof(value_t)));
        // std::cout << ":: " << "size = " << buffer.size() << std::endl;
        ham::offload::get_sync(buffer, value_ptr, buffer.size());
        // std::cout << ":: " << "copied data back" << std::endl;
        // remote free buffer on node
        ham::offload::free(buffer);
        // std::cout << ":: " << "freed data" << std::endl;
        // std::cout << ":: " << *value_ptr << std::endl;
        return value_ptr;
    }

    // NOTE: this is non-const, since it will move out the content
    operator contiguous_container<value_t, deleter_t>()// const
    {
        // TODO/NOTE: nodes might be different
        // we assume that migrations move data to a different node
        // assert(ham::offload::this_node() == _sized_buffer_ptr.node());

        // if (ham::offload::is_host() && )
        // static_assert(std::is_const_v<decltype(value)>);
        // static_assert(std::is_const_v<decltype(this)>);
        // std::cout << "target_migratable<contiguous_container>-conversion on " << ham::offload::this_node() << ": "; detail::print(value); std::cout << std::endl;
        if (mode(_sized_buffer_ptr.node()) == target_migratable_mode::push_data)
        {
            // std::cout << "() push_data" << std::endl;
            // _sized_buffer_ptr has copy-and-swap semantics
            sized_buffer_ptr<value_t> buffer{std::move(_sized_buffer_ptr)};
            return {buffer.size(), buffer.get()};
        }
        // std::cout << "() pull_data" << std::endl;
        sized_buffer_ptr<value_t> buffer{std::move(_sized_buffer_ptr)};
        return {buffer.size(), allocate_and_transfer_data_from_node(buffer)};
    }

    // TODO: add const version
    // https://github.com/noma/ham/issues/12
    sized_buffer_ptr<value_t> & data()
    {
        return _sized_buffer_ptr;
    }

    size_t size() const
    {
        return _sized_buffer_ptr.size();
    }

protected:
    sized_buffer_ptr<value_t> _sized_buffer_ptr;
    target_migratable_mode _mode;
};

}
