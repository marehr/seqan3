#pragma once

#include <seqan3/io/awesome_file/file_base.hpp>
#include <seqan3/io/awesome_file/record_sequence.hpp>

namespace seqan3::awesome
{

// This is our sequence file in.
using sequence_file_in = input_file<record_sequence>;

} // namespace seqan3::awesome
