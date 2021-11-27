#include <algorithm>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

int main()
{
    using namespace std;
    
    string line;
    ifstream inputFile("input.txt");
    using Entry = tuple<int, int, char, char[23]>;
    vector<Entry> entries;
    
    if (!inputFile.is_open())
        return -1;

    while (getline(inputFile, line))
    {
        auto& [min, max, letter, pwd] = entries.emplace_back(Entry{});
        sscanf(line.c_str(), "%d-%d %c: %23s", &min, &max, &letter, pwd);
    }

    inputFile.close();

    // part1
    auto result = count_if(
        begin(entries),
        end(entries),
        [](const auto& entry)
        { 
            const auto& [min, max, letter, pwd] = entry;
            auto c = count(begin(pwd), end(pwd), letter);
            return c >= min && c <= max;
        });

    cout << result << "\n";

    // part2
    result = count_if(
        begin(entries),
        end(entries),
        [](const auto& entry)
        {
            const auto& [a, b, letter, pwd] = entry;
            return (pwd[a-1] == letter) ^ (pwd[b-1] == letter);
        });
   
    cout << result << "\n";

    return 0;
}
