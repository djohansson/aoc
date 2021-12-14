#include <array>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <numeric>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <tuple>

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

template<typename MapT>
struct CompareValue
{
    using value_type = typename MapT::value_type;

    bool operator()(const value_type& left, const value_type& right) const
    {
        return left.second < right.second;
    }
};

}

int main()
{
    using namespace aoc;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    string polymer;
    map<string, char> rules;
    map<char, uint64_t> h;

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

    for_each(begin(rules), end(rules), [](const auto& rule) { cout << get<0>(rule) << " -> " << get<1>(rule) << '\n'; });

    cout << "\nrules.size(): " << rules.size() << "\n\n";

    cout << "polymer: " << polymer << "\n\n";

    map<string, uint64_t> h2;
    for_each(begin(polymer), end(polymer) - 1, [&h, &h2, nextIt = begin(polymer)](auto c) mutable
    {
        nextIt = next(nextIt);
        h[c]++;
        h2[string{c} + *nextIt]++;
    });
    h[*prev(end(polymer))]++;
    
    cout << "h2.size(): " << h2.size() << "\n\n";

    for (unsigned step = 1; step <= 40; ++step) // part1: 10
    {
        auto hc = h;
        auto h2c = h2;

        for_each(begin(h2), end(h2), [&hc, &h2c, &rules](const auto& bucket) mutable
        {
            const auto& [key, val] = bucket;

            if (auto ruleIt = rules.find(key); ruleIt != rules.end())
            {
                const auto& [ruleKey, c] = *ruleIt;

                h2c[key] -= val;

                if (h2c[key] == 0)
                    h2c.erase(key);

                h2c[key.substr(0, 1) + c] += val;
                h2c[string{ c } + key.substr(1, 1)] += val;

                hc[c] += val;
            }
        });
        
        swap(h, hc);
        swap(h2, h2c);

        cout << "h2.size(): " << h2.size() << "\n\n";
    }

    auto minBucket = *min_element(h.begin(), h.end(), CompareValue<decltype(h)>());
    auto maxBucket = *max_element(h.begin(), h.end(), CompareValue<decltype(h)>());

    cout << "\nresult: " << maxBucket.second - minBucket.second << '(' << maxBucket.second << " - " << minBucket.second << ")\n";
    
    return 0;
}
