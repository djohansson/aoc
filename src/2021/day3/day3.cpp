#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

int main()
{
    using namespace std;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    unsigned gamma = 0, epsilon = 0;

    vector<unsigned> numbers;
    size_t bitWidth = 0;
    string line;
    while (getline(inputFile, line, '\n'))
    {
        bitWidth = max(bitWidth, line.size());

        auto& number = numbers.emplace_back();
        for (unsigned bitIt = 0; bitIt < line.size(); bitIt++)
        {
            switch (line[line.size() - bitIt - 1])
            {
            case '1':
                number |= 1 << bitIt;
                break;
            default:
                break;
            }
        }
    }

    auto bitCount = [](const auto& numbers, unsigned bit)
    {
        unsigned count = 0;
        for (const auto& number : numbers)
            count += !!(number & (1 << bit));

        return count;
    };

    // part 1
    for (unsigned bitIt = 0; bitIt < bitWidth; bitIt++)
        gamma |= (numbers.size() <= (bitCount(numbers, bitIt) << 1)) << bitIt;

    epsilon = ~gamma & ((1 << bitWidth) - 1);
    
    cout << gamma << "\n";
    cout << epsilon << "\n";
    cout << gamma * epsilon << "\n";

    // part 2
    array<tuple<vector<unsigned>, bool>, 2> part2data{ make_tuple(numbers, true), make_tuple(numbers, false) };
    auto compare = [](auto a, auto b, bool isLE) { return isLE ? a <= b : a > b; };
    for (auto& [n, isLE] : part2data)
    {
        int bitIt = bitWidth - 1;
        auto nEnd = end(n);
        while (std::distance(begin(n), nEnd) > 1)
        {
            bool criteria = compare(n.size(), size_t(bitCount(n, bitIt) << 1), isLE);
            nEnd = remove_if(begin(n), nEnd, [&bitIt, &criteria](auto number) { return !!(number & (1 << bitIt)) != criteria; });
            bitIt = (bitIt - 1) % bitWidth;
        }
        
        n.resize(std::distance(begin(n), nEnd));
        
        cout << n.back() << "\n";
    }

    cout << get<0>(part2data[0]).back() * get<0>(part2data[1]).back() << "\n";

    return 0;
}
