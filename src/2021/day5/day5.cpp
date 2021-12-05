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
            //cout << p1x << "," << p1y << " -> " << p2x << "," << p2y << "\n";
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
        
        Vec s = { p2x - p1x, p2y - p1y };
        const auto& [sx, sy] = s;
        Vec d = { sgn(sx), sgn(sy) };
        const auto& [dx, dy] = d;

        Vec p = p1;
        auto& [px, py] = p;

        visited[p] += 1;

        while (px != p2x || py != p2y)
        {
            px += dx;
            py += dy;

            visited[p] += 1;
        }
    }

    unsigned result = 0;
    for (const auto& [p, count] : visited)
        result += (count > 1);

    cout << result << "\n";

    return 0;
}
