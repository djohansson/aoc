#include <algorithm>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <vector>

namespace aoc
{

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

}

int main()
{
    using namespace aoc;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    string polymer;
    map<string, char> rules;

    string line;
    bool readTemplate = true;
    while (getline(inputFile, line, '\n'))
    {
        if (readTemplate)
        {
            polymer = line;
        }
        else
        {
            auto s = split(line, ' ');
            rules.emplace(s.front(), s.back().front());
        }

        if (inputFile.peek() == '\n')
        {
            readTemplate = !readTemplate;
            inputFile.get();
        }
    }

    for (const auto& [p, c] : rules)
        cout << p << " -> " << c << '\n';

    cout << "rules.size(): " << rules.size() << "\n";
    cout << "polymer: " << polymer << "\n";

    map<char, uint64_t> h;
    map<string, uint64_t> h2;

    for (auto c : polymer)
        h[c]++;

    for_each(begin(polymer), end(polymer) - 1, [&h2, nextIt = begin(polymer)](auto c) mutable
    {
        h2[string{c} + *(++nextIt)]++;
    });
    
    cout << "h.size(): " << h.size() << "\n";
    cout << "h2.size(): " << h2.size() << "\n\n";

    for (auto step = 1; step <= 40; ++step) // part1: step <= 10
    {
        auto hc = h;
        auto h2c = h2;

        for_each(begin(h2), end(h2), [&hc, &h2c, &rules](const auto& bucket)
        {
            const auto& [key, val] = bucket;
            auto c = rules[key];

            h2c[key.substr(0, 1) + string{c}] += val;
            h2c[string{c} + key.substr(1, 1)] += val;
            h2c[key] -= val;

            if (h2c[key] == 0)
                h2c.erase(key);

            hc[c] += val;
        });
        
        swap(h, hc);
        swap(h2, h2c);

        cout << "step: " << step << "\n";
        cout << "h.size(): " << h.size() << "\n";
        cout << "h2.size(): " << h2.size() << "\n\n";
    }

    auto [minBucket, maxBucket] = minmax_element(begin(h), end(h), [](const auto& lhs, const auto& rhs)
    {
        return lhs.second < rhs.second;
    });

    cout << "result: " << maxBucket->second - minBucket->second <<
        " (" << maxBucket->second << " - " << minBucket->second << ")\n";
    
    return 0;
}
