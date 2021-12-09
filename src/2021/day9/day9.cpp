
#include <algorithm>
#include <iostream>
#include <fstream>
#include <numeric>
#include <string>
#include <vector>
#include <utility>

using namespace std;

using Coords = pair<unsigned, unsigned>;
using Heights = vector<unsigned>;
using CoordSet = vector<Coords>;
using Basin = pair<CoordSet, unsigned>;

unsigned idx(unsigned x, unsigned y, unsigned rowSize)
{
    return y * rowSize + x;
}

unsigned sample(unsigned x, unsigned y, unsigned rowSize, Heights& heights)
{
    return heights[idx(x, y, rowSize)];
};

bool contains(unsigned x, unsigned y, const Basin& basin)
{
    return binary_search(begin(basin.first), end(basin.first), Coords{x, y});
}

unsigned expand(unsigned x, unsigned y, unsigned rowSize, Heights& heights, Basin& basin)
{
    if (contains(x, y, basin))
        return 0;

    auto h = sample(x, y, rowSize, heights);
    if (h >= 9)
        return 0;

    auto& [coords, lp] = basin;
    lp = min(h, lp);
    coords.emplace_back(Coords{x, y});
    sort(begin(coords), end(coords));

    unsigned result = 1u;

    result += expand(x - 1, y, rowSize, heights, basin);
    result += expand(x + 1, y, rowSize, heights, basin);
    result += expand(x, y - 1, rowSize, heights, basin);
    result += expand(x, y + 1, rowSize, heights, basin);
    
    return result;
};

int main()
{
    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    Heights heights;
    string line;
    unsigned colSize = 0;
    unsigned rowSize = 0;

    while (getline(inputFile, line, '\n'))
    {
        rowSize = max(unsigned(line.size()) + 2, rowSize);
        heights.resize(heights.size() + rowSize);
        
        if (colSize++ == 0)
        {
            fill(begin(heights), end(heights), ~0u);
            heights.resize(heights.size() + rowSize);
        }

        auto colIt = end(heights) - rowSize;
        *(colIt++) = ~0u;
        for (auto c : line)
            *(colIt++) = static_cast<unsigned>(c) - 48u;
        *(colIt++) = ~0u;

        if (inputFile.peek() == char_traits<char>::eof())
        {
            colSize += 2;
            heights.resize(heights.size() + rowSize);
            fill(end(heights) - rowSize, end(heights), ~0u);
        }
    }

    vector<pair<Coords, Basin>> basins;

    for (unsigned y = 1; y < colSize - 1; y++)
        for (unsigned x = 1; x < rowSize - 1; x++)
        {
            auto h = sample(x, y, rowSize, heights);
            auto h00 = sample(x - 1, y, rowSize, heights);
            auto h01 = sample(x + 1, y, rowSize, heights);
            auto h10 = sample(x, y - 1, rowSize, heights);
            auto h11 = sample(x, y + 1, rowSize, heights);
            if (h < min(min(h00, h01), min(h10, h11)))
                basins.emplace_back(make_pair(make_pair(x, y), make_pair(CoordSet{}, h)));
        }

    // part 1
    cout << accumulate(begin(basins), end(basins), 0u, [](auto prev, const auto& p) { return prev + p.second.second + 1; }) << "\n";

    // part 2
    vector<unsigned> basinSizes;

    for (auto& [key, basin] : basins)
        basinSizes.emplace_back(expand(key.first, key.second, rowSize, heights, basin));

    sort(begin(basinSizes), end(basinSizes), std::greater<>());

    cout << accumulate(begin(basinSizes), next(begin(basinSizes), 3), 1u, [](auto prev, auto s) { return prev * s; }) << "\n";

    return 0;
}
