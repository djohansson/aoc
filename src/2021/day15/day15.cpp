#include <algorithm>
#include <array>
#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include <vector>
#include <queue>

namespace aoc
{

using namespace std;
using Coord = tuple<uint16_t, uint16_t>;
using Risks = vector<uint16_t>;
using CoordVec = vector<Coord>;
using Path = tuple<CoordVec, uint32_t>; // coord, accumulated risk

struct PathCompare
{
    bool operator()(const Path& a, const Path& b) const
    {
        return get<1>(a) > get<1>(b);
    }
};

using PathQueue = priority_queue<Path, vector<Path>, PathCompare>;

constexpr uint16_t cx_invalid = ~0ui16;
constexpr Coord cx_start = Coord{1, 1};

static inline unsigned idx(unsigned x, unsigned y, unsigned rowSize)
{
    return y * rowSize + x;
}

static inline unsigned idx(const Coord& c, unsigned rowSize)
{
    const auto& [cy, cx] = c;

    return cy * rowSize + cx;
}

template <typename T>
static inline unsigned sample(const Coord& c, unsigned rowSize, const T& data)
{
    return data[idx(c, rowSize)];
};

template <typename T>
static inline unsigned sample(unsigned x, unsigned y, unsigned rowSize, const T& data)
{
    return data[idx(x, y, rowSize)];
};

static inline bool contains(const Coord& c, const CoordVec& vec)
{
    return binary_search(begin(vec), end(vec), c);
}

static inline bool contains(const Coord& c, const PathQueue& pq)
{
    for (auto f = pq; !f.empty(); f.pop())
        if (contains(c, get<0>(f.top())))
            return true;

    return false;
}

template <typename T>
static void traverse(const Coord& goal, unsigned rowSize, const T& data, PathQueue& frontier)
{
    array<Coord, 4> c;
    auto cp = cx_start;

    if (!frontier.empty())
        cp = get<0>(frontier.top()).back();

    if (cp == goal)
        return;

    const auto& [cpy, cpx] = cp;

    c[0] = Coord{ cpy, cpx - 1 };
    c[1] = Coord{ cpy, cpx + 1 };
    c[2] = Coord{ cpy - 1, cpx };
    c[3] = Coord{ cpy + 1, cpx };
    
    {
        array<Path, 4> ps;
        unsigned pSize = 0;

        for (auto& cn : c)
        {
            auto inFrontier = frontier.empty() ? false : contains(cn, frontier);

            if (auto r = sample(cn, rowSize, data); r != cx_invalid && cn != cx_start && !inFrontier)
            {
                auto pc = frontier.empty() ? Path{} : frontier.top();
                auto& [csc, hc] = pc;
                hc += r;
                csc.emplace_back(cn);
                sort(begin(csc), end(csc));
                ps[pSize++] = move(pc);
            }
        }

        for (unsigned pIt = 0; pIt < pSize; ++pIt)
            frontier.emplace(move(ps[pIt]));
    }

    if (frontier.empty())
        return;

    //cout << "\nfrontier:";
    //for (auto dbgq = frontier; !dbgq.empty(); dbgq.pop())
    //{
    //    for (const auto& [dbgy, dbgx] : get<0>(dbgq.top()))
    //        cout << '[' << dbgx << ',' << dbgy << "](" << sample(dbgx, dbgy, rowSize, data) << "){" << get<1>(dbgq.top()) << '}';

    //    cout << ";";
    //}

    cout << "\nfrontier size:" << frontier.size();
    
    //cout << "\ntraverse:";
    //for (const auto& [dbgcy, dbgcx] : get<0>(frontier.top()))
    //    cout << '[' << dbgcx << ',' << dbgcy << ']';

    frontier.pop();

    __attribute__((musttail)) return traverse(goal, rowSize, data, frontier);
}

}

int main()
{
    using namespace aoc;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    Risks risks;
    unsigned colSize = 0;
    unsigned rowSize = 0;

    string line;
    while (getline(inputFile, line, '\n'))
    {
        rowSize = max(unsigned(line.size()) + 2, rowSize);
        risks.resize(risks.size() + rowSize);
        
        if (colSize++ == 0)
        {
            fill(begin(risks), end(risks), cx_invalid);
            risks.resize(risks.size() + rowSize);
        }

        generate_n(end(risks) - rowSize + 1, line.size(), [cIt = line.begin()]() mutable { return static_cast<unsigned>(*cIt++) - 48u; });
        *(end(risks) - 1) = *(end(risks) - rowSize) = cx_invalid;
        
        if (inputFile.peek() == char_traits<char>::eof())
        {
            colSize += 2;
            risks.resize(risks.size() + rowSize);
            fill(end(risks) - rowSize, end(risks), cx_invalid);
        }
    }

    //cout << "\ntraverse:" << "[1,1]";
    PathQueue frontier;
    traverse(Coord{rowSize-2, colSize-2}, rowSize, risks, frontier);
    
    cout << "\npath:\n";
    for (const auto& [cy, cx] : get<0>(frontier.top()))
        cout << '[' << cx << ',' << cy << "](" << sample(cx, cy, rowSize, risks) << ")\n";

    cout << "\nresult: " << get<1>(frontier.top());

    return 0;
}
