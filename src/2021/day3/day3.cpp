#include <fstream>
#include <iostream>
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
    vector<unsigned> colBitCounts;
    string line;
    while (getline(inputFile, line, '\n'))
    {
        colBitCounts.resize(max(colBitCounts.size(), line.size()));

        auto& number = numbers.emplace_back();
        for (unsigned bitIt = 0; bitIt < line.size(); bitIt++)
        {
            switch (line[bitIt])
            {
            case '1':
                number |= 1 << (line.size() - bitIt - 1);
                break;
            default:
                break;
            }
        }
    }

    for (unsigned numberIt = 0; numberIt < numbers.size(); numberIt++)
        for (unsigned bitIt = 0; bitIt < colBitCounts.size(); bitIt++)
            colBitCounts[bitIt] += !!(numbers[numberIt] & (1 << (colBitCounts.size() - bitIt - 1)));

    for (unsigned bitIt = 0; bitIt < colBitCounts.size(); bitIt++)
        gamma |= (colBitCounts[bitIt] > (numbers.size() / 2) ? 1 : 0) << (colBitCounts.size() - bitIt - 1);

    epsilon = ~gamma & ((1 << (colBitCounts.size())) - 1);
    
    cout << gamma << "\n";
    cout << epsilon << "\n";
    cout << gamma * epsilon << "\n";

    return 0;
}
