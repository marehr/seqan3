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

#include <memory>

#include <seqan3/core/platform.hpp>
#include <seqan3/offload/target_migratable.hpp>

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

template<typename contiguous_container_t>
struct target_migratable<contiguous_container_t, std::enable_if_t<is_contiguous_container<contiguous_container_t>::value>>
{
    using value_t = typename std::decay_t<contiguous_container_t>::value_type;

    target_migratable() = default;
    target_migratable(const target_migratable&) = default;
    target_migratable(target_migratable&&) = default;
    target_migratable& operator=(const target_migratable&) = default;
    target_migratable& operator=(target_migratable&&) = default;

    target_migratable(node_t target_node, contiguous_container<value_t> const & contiguous_container)
        : _buffer_ptr{ham::offload::allocate<value_t>(target_node, contiguous_container.size())},
          _size{contiguous_container.size()}
    {
        // std::cout << "target_migratable<const contiguous_container&>-ctor&& on " << ham::offload::this_node() << " -> " << target_node << ": "; detail::print(contiguous_container); std::cout << std::endl;
        ham::offload::put_sync(contiguous_container.begin(), _buffer_ptr, _size); // transfer data to node
    }

    operator contiguous_container<value_t>() const
    {
        // static_assert(std::is_const_v<decltype(value)>);
        // static_assert(std::is_const_v<decltype(this)>);
        // std::cout << "target_migratable<contiguous_container>-conversion on " << ham::offload::this_node() << ": "; detail::print(value); std::cout << std::endl;
        return {_size, _buffer_ptr.get()};
    }

    ham::offload::buffer_ptr<value_t> data() const
    {
        return _buffer_ptr;
    }

    size_t size() const
    {
        return _size;
    }

protected:
    ham::offload::buffer_ptr<value_t> _buffer_ptr;
    size_t _size;
};

}
