
#pragma once

#include <streambuf>

#include <seqan3/offload/contiguous_container.hpp>

template <typename char_t, typename traits = std::char_traits<char_t>>
struct mpi_streambuf : std::basic_streambuf<char_t, traits>
{

};
