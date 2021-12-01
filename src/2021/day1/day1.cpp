#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
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

    unsigned increasedCount = 0;
    for (unsigned numberIt = 1; numberIt < numbers.size(); numberIt++)
    {
        auto prevNumber = numbers[numberIt-1];
        auto number = numbers[numberIt];
        if (prevNumber < number)
            ++increasedCount;
    }

    cout << "part 1 increasedCount: " << increasedCount << "\n";

    increasedCount = 0;
    for (unsigned numberIt = 3; numberIt < numbers.size(); numberIt++)
    {
        int A[3], B[3];
        A[0] = numbers[numberIt-3];
        A[1] = B[0] = numbers[numberIt-2];
        A[2] = B[1] = numbers[numberIt-1];
        B[2] = numbers[numberIt];

        if (accumulate(begin(A), end(A), 0) < accumulate(begin(B), end(B), 0))
            ++increasedCount;
    }

    cout << "part 2 increasedCount: " << increasedCount << "\n";
    
    return 0;
}
