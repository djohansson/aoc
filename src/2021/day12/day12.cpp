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
using CaveMap = map<string, tuple<CaveType, vector<string>>>;

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
        const auto& [type, adjacent]  = val;

        cout << key << ": " << toString(type) << " |";

        for (const auto& cave : adjacent)
            cout << cave << "|";

        cout << "\n";
    }
}

static void traverse(const string& cave, CaveMap& caveMap, vector<string>& path, vector<vector<string>>& trails, bool& twice)
{
    if (cave == "end")
    {
        path.emplace_back(cave);
        trails.emplace_back(path);
        path.pop_back();
        return;
    }

    const auto& [type, adjacent] = caveMap[cave];

    bool twiceSet = false;

    if (type == CaveType::small && find(begin(path), end(path), cave) != end(path))
    {
        if (twice || cave == "start")
            return;

        twice = true;
        twiceSet = true;
    }

    path.emplace_back(cave);

    for (const auto& c : adjacent)
        traverse(c, caveMap, path, trails, twice);
    
    path.pop_back();

    if (twiceSet)
        twice = false;
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

    bool twice = false;
    vector<string> path;
    vector<vector<string>> trails;
    traverse("start", caveMap, path, trails, twice);

    for (const auto& trail : trails)
    {
        cout << trail.front();
        for (auto caveIt = next(begin(trail)); caveIt != end(trail); ++caveIt)
            cout << "," << *caveIt;

        cout << "\n";
    }

    cout << trails.size() << "\n";

    return 0;
}
