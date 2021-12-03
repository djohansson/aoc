#include <fstream>
#include <iostream>
#include <list>
#include <string>
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

    for (unsigned bitIt = 0; bitIt < bitWidth; bitIt++)
    {
        auto c = bitCount(numbers, bitIt);
        gamma |= (c > (numbers.size() / 2) ? 1 : 0) << bitIt;
    }

    epsilon = ~gamma & ((1 << bitWidth) - 1);
    
    cout << gamma << "\n";
    cout << epsilon << "\n";
    cout << gamma * epsilon << "\n";

        // for (unsigned numberIt = 0; numberIt < numbers.size(); numberIt++)
    //     for (unsigned bitIt = 0; bitIt < bitWidth; bitIt++)
    //         colBitCounts[bitIt] += !!(numbers[numberIt] & (1 << (colBitCounts.size() - bitIt - 1)));

    // list<unsigned> o2Numbers(numbers.begin(), numbers.end());
    // vector<unsigned> o2ColBitCounts = colBitCounts;
    // for (unsigned bitIt = 0; bitIt < colBitCounts.size(); bitIt++)
    // {
    //     auto o2NumberIt = o2Numbers.begin();
    //     while (o2NumberIt != o2Numbers.end())
    //     {
    //         if (*o2NumberIt ^ (1 << (colBitCounts.size() - bitIt - 1)))
    //             o2NumberIt = o2Numbers.erase(o2NumberIt);
    //         else
    //             ++o2NumberIt;
    //     }

    // }
    
    

    //vector<unsigned> co2Numbers = numbers;

    return 0;
}
