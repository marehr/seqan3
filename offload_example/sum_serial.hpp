#pragma once

#include <vector>
#include <seqan3/std/span>

int sum_serial(std::span<int const> numbers)
{
    int sum = 0;
    for (int const k : numbers)
        sum += k;
    return sum;
}
