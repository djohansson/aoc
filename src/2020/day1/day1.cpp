#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

int main()
{
    using namespace std;
    namespace rng = std::ranges;
    
    string line;
    ifstream inputFile("input.txt");
    vector<int> numbers;
    pair<int, int> result;

    if (!inputFile.is_open())
        return -1;

    while (getline(inputFile, line))
        numbers.emplace_back(stoi(line));

    inputFile.close();

    if (rng::find_first_of(numbers, numbers, [&result](int a, int b) { result = { a, b }; return (a + b) == 2020; }) == numbers.end())
        return -1;

    const auto& [a, b] = result;

    cout << a << " + " << b << " = " << a + b << "\n";
    cout << a << " * " << b << " = " << a * b << "\n";

    return 0;
}
