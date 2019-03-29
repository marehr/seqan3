#pragma once

int sum_serial(std::vector<int> numbers)
{
    int sum = 0;
    for (auto && k : numbers)
        sum += k;
    return sum;
}
