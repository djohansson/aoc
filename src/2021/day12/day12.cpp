#include <iostream>
#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>

namespace aoc
{

using namespace std;

static const auto sc_hasher = hash<string>{};
static const auto sc_startHash = sc_hasher("start");
static const auto sc_endHash = sc_hasher("end");

enum class CaveType { small, large };

struct Cave
{
    string name = {};
    size_t nameHash = {};
    CaveType type = {};
    vector<size_t> adjacentNameHashes;
};

struct CaveCompare
{
    using is_transparent = Cave;

    inline bool operator()(const unique_ptr<Cave>& a, const unique_ptr<Cave>& b) const
    {
        return a->nameHash < b->nameHash;
    }

    inline bool operator()(size_t a, const unique_ptr<Cave>& b) const
    {
        return a < b->nameHash;
    }

    inline bool operator()(const unique_ptr<Cave>& a, size_t b) const
    {
        return a->nameHash < b;
    }
};

using CaveSet = set<unique_ptr<Cave>, CaveCompare>;

static inline string_view toString(CaveType c)
{
    return c == CaveType::small ? "small" : "large";
}

static inline CaveType getCaveType(const string& s)
{
    auto it = find_if(s.begin(), s.end(), [](auto ch){ return ::isupper(ch); });

    return it == s.end() ? CaveType::small : CaveType::large;
}

static void print(const CaveSet& caves)
{
    for (const auto& cave : caves)
    {
        cout << cave->name << ": " << toString(cave->type) << " |";

        for (auto h : cave->adjacentNameHashes)
            cout << caves.find(h)->get()->name << "|";

        cout << "\n";
    }
}

static void traverse(
    const CaveSet::const_iterator& caveIt,
    CaveSet& caves,
    map<size_t, unsigned>& pathMap,
    bool& twice)
{
    if (caveIt == end(caves))
        return;

    const auto& cave = (*caveIt);

    if (cave->nameHash == sc_endHash)
    {
        pathMap[sc_endHash]++;
        return;
    }

    bool twiceIsSet = false;

    if (cave->type == CaveType::small && pathMap.find(cave->nameHash) != pathMap.end())
    {
        if (twice || cave->nameHash == sc_startHash)
            return;

        twice = true;
        twiceIsSet = true;
    }

    pathMap[cave->nameHash]++;
    
    for (auto h : cave->adjacentNameHashes)
        traverse(caves.find(h), caves, pathMap, twice);

    if (--pathMap[cave->nameHash] == 0)
        pathMap.erase(cave->nameHash);

    if (twiceIsSet)
        twice = false;
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

}

int main()
{
    using namespace aoc;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    CaveSet caves;

    string line;
    while (getline(inputFile, line, '\n'))
    {
        auto s = split(line, '-');
        
        auto c1 = make_unique<Cave>();
        c1->name = s.front();
        c1->nameHash = hash<string>{}(s.front());
        c1->type = getCaveType(s.front());

        auto c2 = make_unique<Cave>();
        c2->name = s.back();
        c2->nameHash = hash<string>{}(s.back());
        c2->type = getCaveType(s.back());

        auto [c1It, c1Inserted] = caves.emplace(move(c1));
        auto [c2It, c2Inserted] = caves.emplace(move(c2));

        (*c1It)->adjacentNameHashes.push_back(c2It->get()->nameHash);
        (*c2It)->adjacentNameHashes.push_back(c1It->get()->nameHash);
    }

    print(caves);

    bool twice = false;
    map<size_t, unsigned> pathMap;
    
    traverse(caves.find(sc_startHash), caves, pathMap, twice);

    cout << pathMap[sc_endHash] << "\n";

    return 0;
}
