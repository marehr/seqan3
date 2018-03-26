
#pragma once

#include <experimental/execution>

namespace seqan3
{

template<class R>
using promise = std::experimental::executors_v1::promise<R>;

template<class R>
using future = std::experimental::executors_v1::future<R>;

template<class R>
using packaged_task = std::experimental::executors_v1::packaged_task<R>;

}
