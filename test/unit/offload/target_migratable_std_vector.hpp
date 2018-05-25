#pragma once

#include <sstream>
#include <seqan3/offload/target_migratable.hpp>

#include <seqan3/offload/contiguous_container.hpp>

namespace seqan3::offload
{
template<typename value_t>
struct target_migratable<std::vector<value_t>>
{
    using contiguous_container_t = contiguous_container<value_t>;
    using migratable_t = target_migratable<contiguous_container_t>;
    // using value_t = typename std::decay_t<contiguous_container_t>::value_type;

    target_migratable() = default;
    target_migratable(const target_migratable&) = default;
    target_migratable(target_migratable&&) = default;
    target_migratable& operator=(const target_migratable&) = default;
    target_migratable& operator=(target_migratable&&) = default;

    target_migratable(node_t target_node, std::vector<value_t> vector)
        : target_migratable()
    {
        // std::cout << "target_migratable<target_node, vector>-ctor on " << ham::offload::this_node() << " -> " << target_node << ": "; detail::print(vector); std::cout << std::endl;
        contiguous_container_t contiguous_container{vector.size(), vector.data()};
        _migrate_sized_buffer = migratable_t{target_node, contiguous_container};
        contiguous_container.release();
        // ham::offload::put_sync(vector.begin(), buffer_ptr, size); // transfer data to node
    }

    // NOTE: data will be moved out
    operator std::vector<value_t>() // const
    {
        contiguous_container_t buffer_ptr = std::move(_migrate_sized_buffer);
        std::vector<value_t> vector(buffer_ptr.begin(), buffer_ptr.end());
        // static_assert(std::is_const_v<decltype(value)>);
        // static_assert(std::is_const_v<decltype(this)>);
        // std::cout << "target_migratable<contiguous_container>-conversion on " << ham::offload::this_node() << ": "; detail::print(value); std::cout << std::endl;
        return vector;
    }

    migratable_t data() const
    {
        return _migrate_sized_buffer;
    }

protected:
    migratable_t _migrate_sized_buffer;
};
}
