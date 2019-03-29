#pragma once

#include <string>
#include <seqan3/io/stream/debug_stream.hpp>

std::string trim(auto & container)
{
    std::ostringstream ostream{};
    seqan3::debug_stream_type stream{ostream};
    stream << (container | std::ranges::view::take(5))
           << "..."
           << (container | std::ranges::view::reverse | std::ranges::view::take(5) | std::ranges::view::reverse);
    return ostream.str();
}
