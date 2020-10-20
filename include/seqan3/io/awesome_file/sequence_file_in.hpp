#pragma once

#include <seqan3/io/awesome_file/file_base.hpp>
#include <seqan3/io/awesome_file/sequence_record.hpp>

namespace seqan3::awesome
{

// This is our sequence file in.
using sequence_file_in = input_file<sequence_record>;

} // namespace seqan3::awesome
