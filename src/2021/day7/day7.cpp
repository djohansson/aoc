#include <algorithm>
#include <iostream>
#include <fstream>
#include <numeric>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

vector<string>& split(const string& s, char delim, vector<string>& elems)
{
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim))
    {
        if (item.length() > 0)
            elems.push_back(item);
    }
    return elems;
}

vector<string> split(const string& s, char delim)
{
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

int main()
{
    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    vector<unsigned> positions;
    unsigned minPos = ~0U, maxPos = 0;

    string line;
    while (getline(inputFile, line, '\n'))
        for (auto numberString : split(line, ','))
        {
            unsigned number = stoul(numberString);
            positions.emplace_back(number);
            minPos = min(number, minPos);
            maxPos = max(number, maxPos);
        }
    
    auto fuelCost = [&positions](unsigned prev, int p)
    {
        auto cost = [](int n)
        {
            //int result = (n - 1) * n / 2;
            int result = 0;

            while (n > 0)
                result += n--;


            return result;
        };
        
        unsigned result = 0;
        for (const auto& p2 : positions)
            result += cost(abs(int(p2 - p)));
        
        return result < prev ? result : prev;
    };

    vector<unsigned> n(maxPos - minPos);
    iota(begin(n), end(n), minPos);
    cout << accumulate(begin(n), end(n), ~0u, fuelCost) << "\n";

    return 0;
}
