#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <tuple>
#include <vector>

template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

int main()
{
    using namespace std;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    static const string sc_lineFormatStr = "%d,%d -> %d,%d";
    using Vec = tuple<int, int>;
    using Line = tuple<Vec, Vec>;
    vector<Line> lines;

    string lineString;
    while (getline(inputFile, lineString, '\n'))
    {
        Line line;
        auto& [p1, p2] = line;
        auto& [p1x, p1y] = p1;
        auto& [p2x, p2y] = p2;
        sscanf(lineString.c_str(), sc_lineFormatStr.c_str(), &p1x, &p1y, &p2x, &p2y);

        //if (p1x == p2x || p1y == p2y) // part1
        {
            cout << p1x << "," << p1y << " -> " << p2x << "," << p2y << "\n";
            lines.emplace_back(move(line));
        }
    }

    using VisitedPoints = map<Vec, unsigned>;
    VisitedPoints visited;

    for (const auto& line : lines)
    {
        const auto& [p1, p2] = line;
        const auto& [p1x, p1y] = p1;
        const auto& [p2x, p2y] = p2;
        
        Vec d = { p2x - p1x, p2y - p1y };
        const auto& [dx, dy] = d;
        Vec s = { sgn(dx), sgn(dy) };
        const auto& [sx, sy] = s;

        Vec p = p1;
        auto& [px, py] = p;

        visited[p] += 1;

        while (px != p2x || py != p2y)
        {
            px += sx;
            py += sy;

            visited[p] += 1;
        }
    }

    unsigned visitedAtLeastTwice = 0;
    for (const auto& v : visited)
    {
        const auto& [p, count] = v;
        if (count > 1)
            ++visitedAtLeastTwice;
    }

    cout << visitedAtLeastTwice << "\n";

    return 0;
}
