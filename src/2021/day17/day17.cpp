#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace aoc
{

using namespace std;

}

int main()
{
    using namespace aoc;

    ifstream inputFile("test.txt");
    if (!inputFile.is_open())
        return -1;

    string line;
    while (getline(inputFile, line, '\n'))
    {
        cout << line << '\n';
    }
    
    return 0;
}
