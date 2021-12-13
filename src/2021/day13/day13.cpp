#include <algorithm>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <ranges>
#include <set>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>
#include <tuple>

namespace aoc
{

using namespace std;
using Coord = tuple<unsigned, unsigned>;
enum class FoldDirection { y, x };
using Fold = tuple<FoldDirection, unsigned>;

static inline string_view toString(FoldDirection d)
{
    switch (d)
    {
    case FoldDirection::y:
        return "y";
    case FoldDirection::x:
        return "x";
    default:
        break;
    }

    return "";
}

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

    struct CoordCompare
    {
        bool operator()(const Coord& a, const Coord& b) const
        {
            const auto& [ax, ay] = a;
            const auto& [bx, by] = b;
            return ay == by ? ax < bx : ay < by;
        }
    };

    using CoordSet = set<Coord, CoordCompare>;
    CoordSet coords;
    vector<Fold> folds;

    string line;
    bool readFolds = false;
    while (getline(inputFile, line, '\n'))
    {
        if (readFolds)
        {
            auto s = split(split(line, ' ').back(), '=');
            folds.emplace_back(s.front() == "y" ? FoldDirection::y : FoldDirection::x, stoul(s.back()));
        }
        else
        {
            auto s = split(line, ',');
            coords.emplace(stoul(s.front()), stoul(s.back()));
        }

        if (inputFile.peek() == '\n')
        {
            readFolds = !readFolds;
            inputFile.get();
        }
    }

    unsigned cmaxx = 0, cmaxy = 0, cminx = ~0u, cminy = ~0u;
    unsigned x = 0, y = 0;

    auto getMinMax = [&cmaxx, &cmaxy, &cminx, &cminy](const auto& coords)
    {
        cmaxx = 0; cmaxy = 0; cminx = ~0u; cminy = ~0u;

        for (const auto& c : coords)
        {
            const auto& [cx, cy] = c;
            cmaxx = max(cmaxx, cx);
            cmaxy = max(cmaxy, cy);
            cminx = min(cminx, cx);
            cminy = min(cminy, cy);
        }

        cout << "min = (" << cminx << "," << cminy << ")\n"
             << "max = (" << cmaxx << "," << cmaxy << ")\n";
    };
    
    auto print = [&cmaxx, &x, &y](const auto& c)
    {
        const auto& [cx, cy] = c;
        while (y <= cy)
        {
            if (y == cy)
            {
                for (; x < cx; x++)
                    cout << '.';

                if (x == cx)
                {
                    cout << '#';
                    if (x >= cmaxx)
                    {
                        x = 0;
                        y += 1;
                        cout << '\n';
                    }
                    else
                    {
                        x++;
                    }
                    return;
                }
            }
            else
            {
                for (; x <= cmaxx; x++)
                    cout << '.';

                if (x >= cmaxx)
                {
                    x = 0;
                    y += 1;
                    cout << '\n';
                }
            }
        }
    };

    auto printRest = [&cmaxx, &cmaxy, &x, &y]
    {
        while (y <= cmaxy)
            while (x <= cmaxx)
            {
                cout << '.';

                if (x == cmaxx)
                {
                    x = 0;
                    y += 1;
                    cout << '\n';
                    break;
                }
                else
                {
                    x++;
                }
            }

        x = 0;
        y = 0;

        cout << '\n';
    };

    auto fold = [](const Fold& f, const CoordSet& c)
    {
        auto project = [&f](const Coord& c)
        {
            const auto& [d, val] = f;
            const auto& [cx, cy] = c;

            switch (d)
            {
            case FoldDirection::y:
                return cy > val ? Coord{ cx, val - (cy - val) } : Coord{ cx, cy };
                break;
            case FoldDirection::x:
                return cx > val ? Coord{ val - (cx - val), cy } : Coord{ cx, cy };
                break;
            default:
                break;
            }

            return Coord{ cx, cy };
        };

        CoordSet result;
        ranges::transform(c, inserter(result, end(result)), project);

        return result;
    };

    cout << "size: " << coords.size() << "\n";
    
    for (const auto& f : folds)
    {
        cout << "fold: " << toString(get<0>(f)) << "," << get<1>(f) << "\n";
        cout << "size: " << coords.size() << "\n";

        coords = fold(f, coords);
    }
    
    getMinMax(coords);
    ranges::for_each(coords, print);
    printRest();   

    return 0;
}
