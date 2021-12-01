#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

int main()
{
    using namespace std;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    using Group = map<char, unsigned>;
    vector<Group> groups;
    groups.emplace_back(Group{});

    string questions;
    while (getline(inputFile, questions, '\n'))
    {
        for (auto q : questions)
            groups.back()[q]++;

        if (inputFile.peek() == '\n')
        {
            groups.emplace_back(Group{});
            continue;
        }
    }

    unsigned s = 0;
    for (const auto& g : groups)
        s += g.size();

    cout << s << "\n";

    return 0;
}
