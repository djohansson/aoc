#include <array>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <ranges>
#include <set>
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

}

int main()
{
    using namespace aoc;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    using Fold = tuple<unsigned, bool>; // val, isY
    using Coord = tuple<unsigned, unsigned>; // y, x
    using CoordSet = set<Coord>;

    CoordSet coords;
    vector<Fold> folds;

    string line;
    bool readFolds = false;
    while (getline(inputFile, line, '\n'))
    {
        if (readFolds)
        {
            auto s = split(split(line, ' ').back(), '=');
            folds.emplace_back(stoul(s.back()), s.front() == "y");
        }
        else
        {
            auto s = split(line, ',');
            coords.emplace(stoul(s.back()), stoul(s.front()));
        }

        if (inputFile.peek() == '\n')
        {
            readFolds = !readFolds;
            inputFile.get();
        }
    }

    auto print = [](const auto& coords)
    {
        struct PrintState
        {
            array<unsigned, 4> bounds = { 0u, 0u, ~0u, ~0u };
            array<unsigned, 2> pos = { 0u, 0u };
        } ps;

        auto getMinMax = [&ps](const auto& coords)
        {
            auto& cmaxx = ps.bounds[0];
            auto& cmaxy = ps.bounds[1];
            auto& cminx = ps.bounds[2];
            auto& cminy = ps.bounds[3];

            cmaxx = 0; cmaxy = 0; cminx = ~0u; cminy = ~0u;

            for (const auto& c : coords)
            {
                const auto& [cy, cx] = c;

                cmaxx = max(cmaxx, cx);
                cmaxy = max(cmaxy, cy);
                cminx = min(cminx, cx);
                cminy = min(cminy, cy);
            }

            cout << "min = (" << cminx << "," << cminy << ")\n"
                 << "max = (" << cmaxx << "," << cmaxy << ")\n";
        };

        auto printNextCoord = [&ps](const auto& c)
        {
            const auto& cmaxx = ps.bounds[0];
            auto& x = ps.pos[0];
            auto& y = ps.pos[1];
            const auto& [cy, cx] = c;

            while (y < cy)
            {
                for (; x <= cmaxx; x++)
                    cout << '.';

                x = 0;
                y += 1;

                cout << '\n';
            }

            for (; x < cx; x++)
                cout << '.';

            cout << '#';

            if (x++ == cmaxx)
            {
                x = 0;
                y += 1;

                cout << '\n';
            }
        };

        auto printRest = [&ps]
        {
            const auto& cmaxx = ps.bounds[0];
            const auto& cmaxy = ps.bounds[1];
            auto& x = ps.pos[0];
            auto& y = ps.pos[1];

            while (y++ <= cmaxy)
            {
                while (x++ < cmaxx)
                    cout << '.';

                x = 0;
                
                cout << ".\n";
            }

            cout << '\n';
        };

        getMinMax(coords);
        ranges::for_each(coords, printNextCoord);
        printRest();
    };

    auto fold = [](const Fold& f, const CoordSet& c)
    {
        auto project = [&f](const Coord& c)
        {
            const auto& [val, d] = f;
            const auto& [cy, cx] = c;

            return d ?
                (cy > val ? Coord{ val - (cy - val), cx } : Coord{ cy, cx }) :
                (cx > val ? Coord{ cy, val - (cx - val) } : Coord{ cy, cx });
        };

        CoordSet result;
        ranges::transform(c, inserter(result, end(result)), project);

        return result;
    };

    cout << "size: " << coords.size() << "\n";

    //print(coords);
    
    for (const auto& f : folds)
    {
        const auto& [val, d] = f;

        cout << "fold: " << (d ? 'y' : 'x') << "," << val << "\n";
        cout << "size: " << coords.size() << "\n";

        coords = fold(f, coords);

        //print(coords);
    }
    
    print(coords);  

    return 0;
}
