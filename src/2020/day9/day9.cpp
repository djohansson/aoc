#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

int main()
{
    constexpr unsigned cx_preambleSize = 25;
    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    vector<uint64_t> numbers;
    string line;
    while (getline(inputFile, line, '\n'))
        numbers.emplace_back(stoull(line));

    auto isValid = [index = 0, &numbers](auto n) mutable 
    {
        auto i = index++;

        if (i < cx_preambleSize)
            return true;

        auto preambleStart = cbegin(numbers) + i - cx_preambleSize;
        auto preambleEnd = cbegin(numbers) + i;

        for (auto it1 = preambleStart; it1 != preambleEnd; ++it1)
        {
            for (auto it2 = preambleStart; it2 != preambleEnd; ++it2)
            {
                if (it1 == it2)
                    continue;

                if (*it1 + *it2 == n)
                    return true;
            }
        }

        return false;
    };

    auto invalid = find_if_not(begin(numbers), end(numbers), isValid);
    if (invalid != end(numbers))
        cout << *invalid << "\n";

    // part 2
    auto rangeStartIt = cbegin(numbers);
    auto rangeEndIt = cend(numbers);
    for (; rangeStartIt != cend(numbers); ++rangeStartIt)
    {
        uint64_t sum = *rangeStartIt;
        for (rangeEndIt = rangeStartIt + 1; rangeEndIt != cend(numbers); ++rangeEndIt)
        {
            sum += *rangeEndIt;
            if (sum > *invalid)
                break;
            else if (sum == *invalid)
                goto part2end;
        }
    }

part2end:
    auto [rangeMinIt, rangeMaxIt] = minmax_element(rangeStartIt, rangeEndIt);
    cout << *rangeMinIt + *rangeMaxIt << "\n";

    return 0;
}

