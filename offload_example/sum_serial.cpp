
#include <iostream>
#include <numeric>
#include <thread>

#include <seqan3/io/stream/debug_stream.hpp>
#include <seqan3/std/ranges>

int sum(std::vector<int> numbers)
{
    seqan3::debug_stream << "vector: " << numbers << std::endl;
    int sum = 0;
    for (auto && k : numbers)
        sum += k;
    return sum;
}

int main(int argc, char **argv)
{
    std::vector<int> numbers{std::ranges::view::iota(0, 100)};
    seqan3::debug_stream << "vector: " << numbers << std::endl;

    int s = sum(numbers);

    std::cout << "result of sum[0,...," << numbers.size() << ") = " << s << '\n';
    return 0;
}
