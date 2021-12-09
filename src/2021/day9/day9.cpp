
#include <iostream>
#include <fstream>
#include <numeric>
#include <string>
#include <sstream>
#include <vector>

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

int main()
{
    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    vector<vector<unsigned>> heights;
    string line;
    while (getline(inputFile, line, '\n'))
    {
        auto& row = heights.emplace_back();
        row.reserve(line.size());
        for (auto c : line)
            row.emplace_back(static_cast<unsigned>(c) - 48);
    }

    vector<unsigned> lp;

    for (unsigned y = 0; y < heights.size(); y++)
        for (unsigned x = 0; x < heights[y].size(); x++)
        {
            int h = heights[y][x];
            if (x == 0 && y == 0)
            {
                if (h < min(heights[y][x+1], heights[y+1][x]))
                    lp.emplace_back(h);
            }
            else if (x == (heights[y].size() - 1) && y == (heights.size() - 1))
            {
                if (h < min(heights[y][x-1], heights[y-1][x]))
                    lp.emplace_back(h);
            }
            else if (x == 0 && y == (heights.size() - 1))
            {
                if (h < min(heights[y][x+1], heights[y-1][x]))
                    lp.emplace_back(h);
            }
            else if (x == (heights[y].size() - 1) && y == 0)
            {
                if (h < min(heights[y+1][x], heights[y][x-1]))
                    lp.emplace_back(h);
            }
            else if (x == 0)
            {
                if (h < min(heights[y][x+1], min(heights[y+1][x], heights[y-1][x])))
                    lp.emplace_back(h);
            }
            else if (x == (heights[y].size() - 1))
            {
                if (h < min(heights[y][x-1], min(heights[y+1][x], heights[y-1][x])))
                    lp.emplace_back(h);
            }
            else if (y == 0)
            {
                if (h < min(min(heights[y][x+1], heights[y][x-1]), heights[y+1][x]))
                    lp.emplace_back(h);
            }
            else if (y == (heights.size() - 1))
            {
                if (h < min(min(heights[y][x-1], heights[y][x+1]), heights[y-1][x]))
                    lp.emplace_back(h);
            }
            else
            {
                if (h < min(min(heights[y][x-1], heights[y][x+1]), min(heights[y-1][x], heights[y+1][x])))
                    lp.emplace_back(h);
            }
        }

    cout << accumulate(begin(lp), end(lp), 0u, [](auto prev, auto p){ return prev + p + 1; }) << "\n";

    return 0;
}
