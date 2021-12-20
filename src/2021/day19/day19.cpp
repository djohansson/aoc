#include <fstream>
#include <iostream>
#include <string>

namespace aoc
{

using namespace std;

}

int main()
{
    using namespace aoc;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    string line;
    while (getline(inputFile, line, '\n'))
    {
    }

    return 0;
}
