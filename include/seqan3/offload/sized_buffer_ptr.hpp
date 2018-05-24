
#pragma once

#include <seqan3/offload/mpi_backend.hpp>

namespace seqan3::offload
{

template<typename value_t>
class sized_buffer_ptr : public ham::offload::buffer_ptr<value_t>
{
public:
    sized_buffer_ptr() = default;
    sized_buffer_ptr(sized_buffer_ptr const &) = default;
    sized_buffer_ptr(sized_buffer_ptr && buffer) :
        ham::offload::buffer_ptr<value_t>{std::move(buffer)},
        size_{std::exchange(buffer.size_, 0u)}
    {};
    sized_buffer_ptr& operator=(sized_buffer_ptr const &) = default;
    sized_buffer_ptr& operator=(sized_buffer_ptr && buffer)
    {
        static_cast<ham::offload::buffer_ptr<value_t>&>(*this) = std::move(buffer);
        size_ = std::exchange(buffer.size_, 0u);
        return *this;
    }

    sized_buffer_ptr(ham::offload::buffer_ptr<value_t> buffer, size_t size)
        : ham::offload::buffer_ptr<value_t>{std::move(buffer)}, size_{size}
    {}

    size_t size() const
    {
        return size_;
    }

private:
    size_t size_;
};

} // namespace seqan3::offload
