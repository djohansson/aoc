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

    unsigned gamma = 0, epsilon = 0, rowCount = 0;
    vector<unsigned> colBitCounts;
    string line;
    while (getline(inputFile, line, '\n'))
    {
        ++rowCount;
        colBitCounts.resize(max(colBitCounts.size(), line.size()));

        for (unsigned bitIt = 0; bitIt < line.size(); bitIt++)
        {
            switch (line[bitIt])
            {
            case '1':
                colBitCounts[bitIt]++;
                break;
            default:
                break;
            }
        }
    }

    for (unsigned bitIt = 0; bitIt < colBitCounts.size(); bitIt++)
    {
        auto bitCount = colBitCounts[bitIt];
        gamma |= (bitCount > rowCount / 2 ? 1 : 0) << (colBitCounts.size() - bitIt - 1);
    }

    epsilon = ~gamma & ((1 << (colBitCounts.size())) - 1);
    
    cout << gamma << "\n";
    cout << epsilon << "\n";
    cout << gamma * epsilon << "\n";

    return 0;
}
