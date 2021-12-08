#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <tuple>
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
    for (const auto& [sp, o] : entries)
        for (const auto& s : o)
            count += (s.size() == 2 || s.size() == 3 || s.size() == 4 || s.size() == 7);

    cout << count << "\n";
    
    return 0;
}
