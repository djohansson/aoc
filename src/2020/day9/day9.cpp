#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

int main()
{
    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    vector<uint64_t> numbers;
    string line;
    while (getline(inputFile, line, '\n'))
        numbers.emplace_back(stoull(line));

    auto isValid = [index = 0, &numbers](auto n) mutable 
    {
        constexpr unsigned preamble = 25;
        auto i = index++;

        if (i < preamble)
            return true;

        auto preambleStart = cbegin(numbers) + i - preamble;
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

    if (auto invalid = find_if_not(begin(numbers), end(numbers), isValid); invalid != end(numbers))
        cout << *invalid << "\n";

    return 0;
}

