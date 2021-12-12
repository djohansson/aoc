#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <string_view>
#include <sstream>
#include <tuple>
#include <utility>
#include <vector>

namespace aoc
{

using namespace std;

enum class CaveType : uint8_t { small, large };
using Paths = vector<string>;
using Trail = vector<string>;
using CaveMap = map<string, tuple<CaveType, Paths>>;

static inline string_view toString(CaveType c)
{
    return c == CaveType::small ? "small" : "large";
}

static inline CaveType getCaveType(const string& s)
{
    auto it = find_if(s.begin(), s.end(), [](auto ch){ return ::isupper(ch); });

    return it == s.end() ? CaveType::small : CaveType::large;
}

static vector<string>& split(const string& s, char delim, vector<string>& elems)
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

static vector<string> split(const string& s, char delim)
{
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

static void print(const CaveMap& caveMap)
{
    for (const auto& [key, val] : caveMap)
    {
        const auto& [type, paths]  = val;

        cout << key << ": " << toString(type) << " |";

        for (const auto& path : paths)
            cout << path << "|";

        cout << "\n";
    }
}

static void traverse(const string& current, CaveMap& caveMap, Trail& trail, unsigned& pathCount)
{
    if (current == "end")
    {
        pathCount += 1;
        return;
    }

    const auto& [type, next] = caveMap[current];

    if (type == CaveType::small && find(begin(trail), end(trail), current) != end(trail))
        return;

    trail.emplace_back(current);

    for (const auto& cave : next)
        traverse(cave, caveMap, trail, pathCount);
    
    trail.pop_back();
}

}

int main()
{
    using namespace aoc;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    CaveMap caveMap;
    string line;
    while (getline(inputFile, line, '\n'))
    {
        auto s = split(line, '-');
        auto& [sfType, sfPaths] = caveMap[s.front()];
        auto& [sbType, sbPaths] = caveMap[s.back()];
        sfType = getCaveType(s.front());
        sbType = getCaveType(s.back());
        sfPaths.emplace_back(s.back());
        sbPaths.emplace_back(s.front());
    }

    print(caveMap);

    unsigned pathCount = 0;
    Trail trail;
    traverse("start", caveMap, trail, pathCount);

    cout << pathCount << "\n";

    return 0;
}
