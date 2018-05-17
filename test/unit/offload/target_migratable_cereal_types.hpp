#pragma once

#include <sstream>

#include <seqan3/offload/target_migratable.hpp>

#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>

#include <seqan3/offload/contiguous_container.hpp>

namespace seqan3::detail
{

template <typename char_t, typename traits_t = std::char_traits<char_t>, typename allocator_t = std::allocator<char_t>>
struct stringbuf_range
{
    struct my_stringbuf: std::stringbuf
    {
        using std::stringbuf::pbase;
        using std::stringbuf::pptr;
    };

    char_t* begin() const noexcept
    {
        return reinterpret_cast<my_stringbuf*>(buffer)->pbase();
    }

    char_t* end() const noexcept
    {
        return reinterpret_cast<my_stringbuf*>(buffer)->pptr();
    }

    size_t size() const noexcept
    {
        return std::distance(begin(), end());
    }

    std::basic_stringbuf<char_t, traits_t, allocator_t>* buffer;
};
}

namespace seqan3::offload
{
template<typename value_t>
struct target_migratable<std::vector<value_t>>
{
    using char_t = typename std::ostringstream::char_type;
    using contiguous_container_t = contiguous_container<char_t>;
    using migratable_t = target_migratable<contiguous_container_t>;
    // using value_t = typename std::decay_t<contiguous_container_t>::value_type;

    target_migratable() = default;
    target_migratable(const target_migratable&) = default;
    target_migratable(target_migratable&&) = default;
    target_migratable& operator=(const target_migratable&) = default;
    target_migratable& operator=(target_migratable&&) = default;

    target_migratable(node_t target_node, std::vector<value_t> /*const &*/ vector)
        : target_migratable()
    {
        std::ostringstream ostream{std::ios::binary};
        cereal::BinaryOutputArchive oarchive{ostream};
        oarchive(vector);

        seqan3::detail::stringbuf_range<char_t> stringbuf_range{ostream.rdbuf()};

        // std::cout << "target_migratable<target_node, vector>-ctor on " << ham::offload::this_node() << " -> " << target_node << ": "; detail::print(vector); std::cout << std::endl;
        contiguous_container_t contiguous_container{stringbuf_range.size(), stringbuf_range.begin()};
        _migrate_sized_buffer = migratable_t{target_node, contiguous_container};
        contiguous_container.release();
        // ham::offload::put_sync(vector.begin(), buffer_ptr, size); // transfer data to node
    }

    operator std::vector<value_t>() const
    {
        std::vector<value_t> vector;

        contiguous_container_t buffer_ptr = _migrate_sized_buffer;

        std::istringstream istream{std::ios::binary};
        istream.rdbuf()->pubsetbuf(buffer_ptr.begin(), buffer_ptr.size());
        cereal::BinaryInputArchive iarchive{istream};
        iarchive(vector);
        // std::vector<value_t> vector(buffer_ptr.begin(), buffer_ptr.end());
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
