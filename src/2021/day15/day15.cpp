#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include <vector>
#include <queue>

namespace aoc
{

using namespace std;
using namespace std::chrono;

using Coord = tuple<uint16_t, uint16_t>;
using Risks = vector<uint8_t>;
using Cache = vector<uint8_t>;
using CoordVec = vector<Coord>;
using Path = tuple<CoordVec, uint32_t>; // coord, accumulated risk

struct PathCompare
{
    bool operator()(const Path& a, const Path& b) const
    {
        return get<1>(a) > get<1>(b);
    }
};

class SearchContext
{
    using PathQueue = priority_queue<Path, vector<Path>, PathCompare>;

public:

    SearchContext(Coord&& goal) : myGoal(exchange(goal, {})) {}

    auto& frontier() { return myFrontier; }
    const auto& frontier() const { return myFrontier; }

    auto& goal() { return myGoal; }
    const auto& goal() const { return myGoal; }
    
private:

    Coord myGoal;
    PathQueue myFrontier;
};

constexpr uint8_t cx_invalid = ~0ui8;
constexpr Coord cx_start = Coord{1, 1};

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

// static inline bool contains(const Coord& c, const CoordVec& vec)
// {
//     return binary_search(begin(vec), end(vec), c);
// }

// static inline bool contains(const Coord& c, const SearchContext& s)
// {
//     for (auto f = s.frontier(); !f.empty(); f.pop())
//         if (contains(c, get<0>(f.top())))
//             return true;

//     return false;
// }

template <typename T>
static void traverse(unsigned rowSize, const T& data, SearchContext& s)
{
    static Cache s_cache;
    static bool s_cache_ready = false;
    if (!s_cache_ready)
    {
        s_cache.resize(data.size());
        s_cache[idx(cx_start, rowSize)] = cx_invalid;
        s_cache_ready = true;
    }

    array<Coord, 4> c;
    auto cp = cx_start;

    if (!s.frontier().empty())
        cp = get<0>(s.frontier().top()).back();

    if (cp == s.goal())
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
            if (auto r = sample(cn, rowSize, data);
                r != cx_invalid &&
                //cn != cx_start &&
                s_cache[idx(cn, rowSize)] == 0 /*&& 
                (s.frontier().empty() || !contains(cn, s))*/)
            {
                auto pc = s.frontier().empty() ? Path{} : s.frontier().top();
                auto& [csc, hc] = pc;
                hc += r;
                csc.emplace_back(cn);
                s_cache[idx(cn, rowSize)]++;
                //sort(begin(csc), end(csc));
                ps[pSize++] = move(pc);

                //cout << "\nadd cache:" << static_cast<unsigned>(s_cache[idx(cn, rowSize)]);
            }
        }

        for (unsigned pIt = 0; pIt < pSize; ++pIt)
            s.frontier().emplace(move(ps[pIt]));
    }

    if (s.frontier().empty())
        return;

    //cout << "\nfrontier:";
    //for (auto dbgq = frontier; !dbgq.empty(); dbgq.pop())
    //{
    //    for (const auto& [dbgy, dbgx] : get<0>(dbgq.top()))
    //        cout << '[' << dbgx << ',' << dbgy << "](" << sample(dbgx, dbgy, rowSize, data) << "){" << get<1>(dbgq.top()) << '}';

    //    cout << ";";
    //}

    //cout << "\nfrontier size:" << s.frontier().size();
    
    //cout << "\ntraverse:";
    //for (const auto& [dbgcy, dbgcx] : get<0>(frontier.top()))
    //    cout << '[' << dbgcx << ',' << dbgcy << ']';

    s.frontier().pop();
    //cout << "\npop cache:" << static_cast<unsigned>(s_cache[idx(cp, rowSize)]);

    __attribute__((musttail)) return traverse(rowSize, data, s);
}

}

int main()
{
    using namespace aoc;

    auto parseStart = high_resolution_clock::now();

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    Risks risks;
    unsigned colSize = 0;
    unsigned rowSize = 0;

    constexpr unsigned cx_tileCount = 5; // part1: 1

    string line;
    while (getline(inputFile, line, '\n'))
    {
        auto sx = static_cast<unsigned>(line.size());

        rowSize = max((sx * cx_tileCount) + 2, rowSize);
        risks.resize(risks.size() + rowSize);
        
        if (colSize++ == 0)
        {
            fill(begin(risks), end(risks), cx_invalid);
            risks.resize(risks.size() + rowSize);
        }

        generate_n(end(risks) - rowSize + 1, rowSize - 2, [n = 0, cBegin = line.begin(), cIt = line.begin(), cEnd = line.end()]() mutable
        {
            if (cIt == cEnd)
            {
                n++;
                cIt = cBegin;
            }

            auto val = (static_cast<unsigned>(*cIt++) - 48u + n - 1) % 9 + 1;

            return val;
        });
        *(end(risks) - 1) = *(end(risks) - rowSize) = cx_invalid;
        
        if (inputFile.peek() == char_traits<char>::eof())
        {
            auto sy = colSize;
            colSize = (sy * cx_tileCount) + 2;
            risks.resize(colSize * rowSize);

            auto srcIt = begin(risks) + rowSize;
            auto dstIt = begin(risks) + rowSize + (rowSize * sy);

            for (unsigned i = 1; i < cx_tileCount; i++)
            {
                for (unsigned y = 0; y < sy; y++)
                {
                    *(dstIt) = *(dstIt + rowSize - 1) = cx_invalid;

                    srcIt += 1;
                    dstIt += 1;

                    generate_n(dstIt, (rowSize - 2), [srcIt]() mutable
                    {
                        return *(srcIt++) % 9 + 1;
                    });
                    
                    srcIt += (rowSize - 2);
                    srcIt += 1;
                    dstIt += (rowSize - 2);
                    dstIt += 1;
                }
            }

            fill(end(risks) - rowSize, end(risks), cx_invalid);
        }
    }

    auto parseEnd = high_resolution_clock::now();

    cout << "parse: " << duration_cast<microseconds>(parseEnd - parseStart).count() << " microseconds";

    auto runStart = high_resolution_clock::now();

    // for (unsigned y = 0; y < colSize; y++)
    // {
    //    for (unsigned x = 0; x < rowSize; x++)
    //    {
    //        auto r = sample(Coord{ y, x }, rowSize, risks);
    //        cout << (r != cx_invalid ? static_cast<char>(r + 48) : '#');
    //    }
    //    cout << '\n';
    // }

    //cout << "\ntraverse:" << "[1,1]";
    SearchContext s(Coord{rowSize-2, colSize-2});
    traverse(rowSize, risks, s);
    
    // cout << "\npath:\n";
    // for (const auto& c : get<0>(s.frontier().top()))
    //     cout << '[' << get<1>(c) << ',' << get<0>(c)<< "](" << sample(c, rowSize, risks) << ")\n";

    cout << "\nresult: " << get<1>(s.frontier().top());

    auto runEnd = high_resolution_clock::now();

    cout << "\ntime: " << duration_cast<microseconds>(runEnd - runStart).count() << " microseconds";

    return 0;
}
