#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <utility>

int main()
{
    using namespace std;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    using Questions = map<char, unsigned>;
    vector<pair<Questions, unsigned>> groups; // questions, group size
    groups.emplace_back(make_pair(Questions{}, 0u));

    string line;
    while (getline(inputFile, line, '\n'))
    {
        auto& [questions, groupSize] = groups.back();

        for (auto q : line)
            questions[q]++;

        groupSize++;

        if (inputFile.peek() == '\n')
        {
            inputFile.get();
            groups.emplace_back(make_pair(Questions{}, 0u));
            continue;
        }
    }

    unsigned s = 0;
    for (const auto& [questions, groupSize] : groups)
        s += questions.size();

    cout << "part1: " << s << "\n";

    s = 0;
    for (const auto& [questions, groupSize] : groups)
        for (const auto& [question, yesCount] : questions)
            s += (yesCount == groupSize);

    cout << "part2: " << s << "\n";

    return 0;
}
