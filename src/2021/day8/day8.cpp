
#include <array>
#include <algorithm>
#include <iostream>
#include <fstream>
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

    using SignalPatterns = vector<string>;
    using OutputValue = vector<string>;
    using Entry = tuple<SignalPatterns, OutputValue>;

    vector<Entry> entries;
    string line;
    while (getline(inputFile, line, '\n'))
    {
        auto s = split(line, '|');
        auto& [sp, o] = entries.emplace_back();
        sp = split(*s.begin(), ' ');
        o = split(*next(s.begin()), ' ');
    }

    unsigned count = 0;
    for (auto& [sp, o] : entries)
    {
        for (auto& p : sp)
            sort(p.begin(), p.end());

        for (auto& s : o)
        {
            sort(s.begin(), s.end());
            count += (s.size() == 2 || s.size() == 3 || s.size() == 4 || s.size() == 7);
        }
    }

    cout << "Count: " << count << "\n"; // part 1

    unsigned sum = 0;
    for (const auto& [sp, so] : entries)
    {
        array<string, 10> patterns;

        for (const auto& p : sp)
        {
            if (p.size() == 2)
                patterns[1] = p;
            else if (p.size() == 3)
                patterns[7] = p;
            else if (p.size() == 4)
                patterns[4] = p;
            else if (p.size() == 7)
                patterns[8] = p;
        }

        for (const auto& p : sp)
        {
            if (p.size() == 5) // 2, 3, 5
            {
                const auto& seven = patterns[7];
                string iseven;
                set_intersection(begin(seven), end(seven), begin(p), end(p), back_inserter(iseven));
                if (iseven.size() == 2) // 2, 5
                {
                    const auto& four = patterns[4];
                    string ifour;
                    set_intersection(begin(four), end(four), begin(p), end(p), back_inserter(ifour));
                    if (ifour.size() == 2) // 2
                        patterns[2] = p;
                    else // 5
                        patterns[5] = p;

                }
                else if (iseven.size() == 3) // 3
                    patterns[3] = p;
                else return -1;
            }
            else if (p.size() == 6) // 0, 6, 9
            {
                const auto& seven = patterns[7];
                string iseven;
                set_intersection(begin(seven), end(seven), begin(p), end(p), back_inserter(iseven));
                if (iseven.size() == 3) // 0, 9
                {
                    const auto& four = patterns[4];
                    string ifour;
                    set_intersection(begin(four), end(four), begin(p), end(p), back_inserter(ifour));
                    if (ifour.size() == 4) // 9
                        patterns[9] = p;
                    else // 0
                        patterns[0] = p;

                }
                else if (iseven.size() == 2) // 6
                    patterns[6] = p;
                else return -1;
            }
        }

        vector<unsigned> digits;
        for (const auto& s : so)
            for (unsigned pIt = 0; pIt < patterns.size(); ++pIt)
                if (s == patterns[pIt])
                    digits.emplace_back(pIt);

        unsigned value = 0;
        unsigned base = 1;
        for (auto it = digits.crbegin(); it != digits.crend(); ++it)
        {
            value += *it * base;
            base *= 10;
        }

        sum += value;
    }

    cout << "Sum: " << sum << "\n"; // part 2
    
    return 0;
}
