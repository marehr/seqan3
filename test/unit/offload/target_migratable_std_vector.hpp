#pragma once

#include <sstream>

#include <seqan3/offload/contiguous_container.hpp>
#include <seqan3/offload/target_migratable.hpp>
#include <seqan3/offload/target_function.hpp>

namespace seqan3::offload
{

template<typename value_t, target_migratable_mode mode>
struct target_migratable<std::vector<value_t>, mode>
{
    using contiguous_container_t = contiguous_container<value_t, no_delete<value_t>>;
    using migratable_t = target_migratable<contiguous_container_t, mode>;

    target_migratable() = default;
    target_migratable(const target_migratable&) = default;
    target_migratable(target_migratable&&) = default;
    target_migratable& operator=(const target_migratable&) = default;
    target_migratable& operator=(target_migratable&&) = default;

    template<typename = typename std::enable_if_t<mode == target_migratable_mode::push_data>>
    target_migratable(node_t target_node, std::vector<value_t> const & vector)
        : target_migratable{}
    {
        // std::cout << "target_migratable<target_node, vector>-ctor on " << ham::offload::this_node() << " -> " << target_node << ": "; detail::print(vector); std::cout << std::endl;

        static_assert(!contiguous_container_t::is_owning(), "contiguous_container_t is non-owning");

        // TODO: Why is const ref not allowed? One problem is that vector.data()
        // will be `const value_t*`. Can `contiguous_container` be adopted to cope
        // with const version? Can we remove const_cast<value_t*>()?
        value_t* data_ptr = const_cast<value_t*>(vector.data());
        contiguous_container_t contiguous_container{vector.size(), data_ptr};
        _container_migration = migratable_t{target_node, contiguous_container};
    }

    template<typename = typename std::enable_if_t<mode == target_migratable_mode::pull_data>>
    target_migratable(std::vector<value_t> vector)
        : target_migratable{}
    {
        // std::cout << "target_migratable<target_node, vector>-ctor on " << ham::offload::this_node() << " -> " << target_node << ": "; detail::print(vector); std::cout << std::endl;
        static_assert(!contiguous_container_t::is_owning(), "contiguous_container_t is non-owning");

        contiguous_container_t contiguous_container{vector.size(), vector.data()};
        _container_migration = migratable_t{contiguous_container};

        // make vector for pull persistent
        _persistent_vector = make_persistent(std::move(vector));
    }

    ham::offload::buffer_ptr<std::vector<value_t>> make_persistent(std::vector<value_t> vector)
    {
        auto ptr = new std::vector<value_t>;
        *ptr = std::move(vector);

        return {ptr, ham::offload::this_node()};
    }

    // NOTE: data will be moved out
    operator std::vector<value_t>() // const
    {
        static_assert(!contiguous_container_t::is_owning(), "contiguous_container_t is non-owning");

        contiguous_container_t container = std::move(_container_migration);
        std::vector<value_t> vector(container.begin(), container.end());

        // TODO: fuse delete/free messages?
        // free persistent vector on node
        if constexpr(mode == target_migratable_mode::pull_data)
        {
            if (_persistent_vector.get() != nullptr)
                offload::free(_persistent_vector, offload::default_delete<std::vector<value_t>>{});

            _persistent_vector = {};
        }

        return vector;
    }

    migratable_t data() const
    {
        return _container_migration;
    }

protected:
    migratable_t _container_migration;
    ham::offload::buffer_ptr<std::vector<value_t>> _persistent_vector;
};

} // namespace seqan3::offload
