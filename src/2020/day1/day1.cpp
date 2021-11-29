#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

int main()
{
    using namespace std;
    
    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    vector<int> numbers;
    string line;
    while (getline(inputFile, line))
        numbers.emplace_back(stoi(line));

    inputFile.close();

    optional<tuple<int, int, int>> result;
    for (auto i : numbers)
        for (auto j : numbers)
            if (i + j == 2020)
            {
                result = { i, j, 0 };
                goto part1_printresult;
            }

part1_printresult:

    if (result.has_value())
    {
        const auto& [i, j, unused] = result.value();

        cout << i << " + " << j << " = " << i + j << "\n";
        cout << i << " * " << j << " = " << i * j << "\n";
    }
    else return -1;

    result = nullopt;

    for (auto i : numbers)
        for (auto j : numbers)
        {
            auto sum = i + j;
            if (sum <= 2020)
            {
                for (auto k : numbers)
                    if (i + j + k == 2020)
                    {
                        result = { i, j, k };
                        goto part2_printresult;
                    }
            }
        }

part2_printresult:

    if (result.has_value())
    {
        const auto& [i, j, k] = result.value();

        cout << i << " + " << j << " + " << k << " = " << i + j + k << "\n";
        cout << i << " * " << j << " * " << k << " = " << i * j * k << "\n";
    }
    else return -1;

    return 0;
}
