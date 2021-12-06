#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <numeric>
#include <tuple>
#include <vector>

std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        if (item.length() > 0)
            elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string& s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

int main()
{
    using namespace std;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    using TimerMap = map<uint64_t, uint64_t>; // timer, count
    TimerMap fishes;
    string line;
    while (getline(inputFile, line, '\n'))
        for (auto number : split(line, ','))
            fishes[stoul(number)]++;

    cout << "Initial state: ";
    for (auto& [timer, count] : fishes)
        cout << "[" << timer << "," << count << "]";
    
    cout << "\n";

    for (uint64_t day = 1; day <= 256; day++) // part1: 80
    {
        TimerMap ticked;

        auto expired = fishes.extract(0);
        
        for (auto it = fishes.begin(); it != fishes.end(); ++it)
        {
            auto& [timer, count] = *it;
            ticked[timer - 1] = count;
        }
        
        if (expired)
        {
            ticked[6] += expired.mapped();
            ticked[8] += expired.mapped();
        }

        fishes.swap(ticked);

        cout << "After " << day << " day: ";

        for (auto& [timer, count] : fishes)
            cout << "[" << timer << "," << count << "]";

        uint64_t sum = std::accumulate(
            std::begin(fishes),
            std::end(fishes),
            0ll,
            [](const auto prev, const auto& fish)
            { return prev + fish.second; });

        cout << "\nSum: " << sum << "\n";
    }

    return 0;
}
