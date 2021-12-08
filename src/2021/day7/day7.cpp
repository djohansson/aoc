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

    vector<unsigned> ps;
    string line;
    while (getline(inputFile, line, '\n'))
        for (auto str : split(line, ','))
            ps.emplace_back(stoul(str));

    auto [minp, maxp] = minmax_element(begin(ps), end(ps));
    vector<unsigned> n(*maxp - *minp);
    iota(begin(n), end(n), *minp);
    auto result = accumulate(begin(n), end(n), ~0u, [&ps](auto prev, auto p)
    {
        return min(prev, accumulate(begin(ps), end(ps), 0u,[&p](auto prev, auto p2)
        {
            //auto cost = [](int n) { return n; }; // part 1
            auto cost = [](int n) { return (n * (n + 1)) / 2; };
            return prev + cost(abs(static_cast<int>(p2 - p)));
        }));
    });
    
    cout << result << "\n";

    return 0;
}
