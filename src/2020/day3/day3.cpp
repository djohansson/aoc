#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    using namespace std;
 
    string line;
    ifstream inputFile("input.txt");
 
    if (!inputFile.is_open())
        return -1;

    constexpr uint8_t c_colCount = 31;
    static const string sc_lineFormatStr = "%" + to_string(c_colCount) + "c";

    using Row = array<char, c_colCount>;
    vector<Row> rows;
    while (getline(inputFile, line))
    {
        auto& cols = rows.emplace_back(Row{});
        sscanf(line.c_str(), sc_lineFormatStr.c_str(), cols.data());
    }

    inputFile.close();

    // part 1

    uint32_t treeCount = 0;
    for (uint32_t rowIndex = 1, colIndex = 3; rowIndex < rows.size(); rowIndex++, colIndex = (colIndex + 3) % c_colCount)
        treeCount += (rows[rowIndex][colIndex] == '#' ? 1 : 0);

    cout << treeCount << "\n";

    // part 2

    constexpr array<array<uint8_t, 2>, 5> slopes =
    {{
        {1ui8, 1ui8},
        {3ui8, 1ui8},
        {5ui8, 1ui8},
        {7ui8, 1ui8},
        {1ui8, 2ui8},
    }};

    uint64_t treeProduct = 1;
    for (uint8_t slopeIndex = 0; slopeIndex < slopes.size(); slopeIndex++)
    {
        const auto& slope = slopes[slopeIndex];

        treeCount = 0;
        for (uint32_t rowIndex = slope[1], colIndex = slope[0]; rowIndex < rows.size(); rowIndex += slope[1], colIndex = (colIndex + slope[0]) % c_colCount)
            treeCount += (rows[rowIndex][colIndex] == '#' ? 1 : 0);

        treeProduct *= treeCount;

        cout << treeCount << "\n";
    }

    cout << treeProduct << "\n";
 
    return 0;
}
