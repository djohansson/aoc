#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

int main()
{
    using namespace std;
    
    string line;
    ifstream inputFile("input.txt");
    
    if (!inputFile.is_open())
        return -1;

    constexpr auto c_maxPwdSize = 23u;
    static const string sc_lineFormatStr = "%d-%d %c: %" + to_string(c_maxPwdSize) + "s";

    using Entry = tuple<int, int, char, char[c_maxPwdSize]>;
    vector<Entry> entries;
    while (getline(inputFile, line))
    {
        auto& [min, max, letter, pwd] = entries.emplace_back(Entry{});
        sscanf(line.c_str(), sc_lineFormatStr.c_str(), &min, &max, &letter, pwd);
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
