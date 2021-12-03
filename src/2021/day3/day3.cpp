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

    // part 1
    for (unsigned bitIt = 0; bitIt < bitWidth; bitIt++)
        gamma |= (numbers.size() <= (bitCount(numbers, bitIt) << 1)) << bitIt;

    epsilon = ~gamma & ((1 << bitWidth) - 1);
    
    cout << gamma << "\n";
    cout << epsilon << "\n";
    cout << gamma * epsilon << "\n";

    // part 2

    list<unsigned> o2Numbers(numbers.begin(), numbers.end());
    list<unsigned> co2Numbers(numbers.begin(), numbers.end());
    
    int bitIt = bitWidth - 1;
    while (o2Numbers.size() > 1)
    {
        bool criteria = o2Numbers.size() <= (bitCount(o2Numbers, bitIt) << 1);
        o2Numbers.remove_if([&bitIt, &criteria](auto number) { return !!(number & (1 << bitIt)) != criteria; });
        bitIt = (bitIt - 1) % bitWidth;
    }

    bitIt = bitWidth - 1;
    while (co2Numbers.size() > 1)
    {
        bool criteria = co2Numbers.size() > (bitCount(co2Numbers, bitIt) << 1);
        co2Numbers.remove_if([&bitIt, &criteria](auto number) { return !!(number & (1 << bitIt)) != criteria; });
        bitIt = (bitIt - 1) % bitWidth;
    }
    
    cout << o2Numbers.back() << "\n";
    cout << co2Numbers.back() << "\n";
    cout << o2Numbers.back() * co2Numbers.back() << "\n";

    return 0;
}
