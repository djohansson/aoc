#include <iostream>
#include <fstream>
#include <map>
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

    using TimerMap = map<unsigned, uint64_t>; // timer, count
    TimerMap fishes;

    string line;
    while (getline(inputFile, line, '\n'))
        for (auto number : split(line, ','))
            fishes[stoul(number)]++;

    cout << "Initial state: ";
    for (const auto& [timer, count] : fishes)
        cout << "[" << timer << "," << count << "]";

    for (unsigned day = 1; day <= 256; ++day) // part1: 1 - 80
    {
        auto expired = fishes.extract(0);
        
        TimerMap ticked;

        for (const auto& [timer, count] : fishes)
            ticked[timer - 1] = count;
        
        if (expired)
        {
            ticked[6] += expired.mapped();
            ticked[8] += expired.mapped();
        }

        fishes.swap(ticked);

        cout << "\nAfter " << day << " day(s): ";
        for (const auto& [timer, count] : fishes)
            cout << "[" << timer << "," << count << "]";
    }

    cout << "\nSum: " << 
        accumulate(
            begin(fishes),
            end(fishes),
            0ll,
            [](auto prev, const auto& fish) { return prev + fish.second; });

    return 0;
}
