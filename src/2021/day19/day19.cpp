#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>

namespace aoc
{

using namespace std;

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

    ifstream inputFile("test.txt");
    if (!inputFile.is_open())
        return -1;

    using Coord = tuple<int16_t, int16_t, int16_t>;
    using Scanner = vector<Coord>;

    vector<Scanner> scanners;

    string line;
    while (getline(inputFile, line, '\n'))
    {
        if (line.empty())
            continue;

        if (line.compare(0, 3, "---") == 0)
        {
            scanners.emplace_back();
            getline(inputFile, line, '\n');
        }

        auto coordsStr = split(line, ',');
        if (coordsStr.size() != 3)
            return -1;

        auto& scanner = scanners.back();

        scanner.emplace_back(Coord{stoi(coordsStr[0]), stoi(coordsStr[1]), stoi(coordsStr[2])});
    }

    unsigned scannerIt = 0;
    for (const auto& scanner : scanners)
    {
        cout << "--- scanner " << scannerIt++ << " ---\n";

        for (const auto& [x, y, z] : scanner)
        {
            cout << x << ',' << y << ',' << z << '\n';
        }
    }

    return 0;
}
