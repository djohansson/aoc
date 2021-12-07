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
            maxPos = min(number, maxPos);
        }
    
    auto fuelCost = [&positions](unsigned prev, int p)
    {
        unsigned cost = 0;
        for (const auto& p2 : positions)
            cost += abs(int(p2 - p));
        
        return cost < prev ? cost : prev;
    };

    cout << accumulate(begin(positions), end(positions), ~0u, fuelCost) << "\n";

    return 0;
}
