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

    int pos = 0, depth1 = 0, depth2 = 0, aim = 0;
    string line;
    while (getline(inputFile, line, '\n'))
    {
        if (line.find("forward") == 0)
        {
            int count = stoi(line.substr(7));
            pos += count;
            depth2 += aim * count;
        }
        else if (line.find("down") == 0)
        {
            int count = stoi(line.substr(4));
            depth1 += count;
            aim += count;
        }
        else if (line.find("up") == 0)
        {
            int count = stoi(line.substr(2));
            depth1 -= count;
            aim -= count;
        }
    }

    cout << pos << "\n";
    cout << depth1 << "\n";
    cout << depth2 << "\n";
    cout << pos * depth1 << "\n";
    cout << pos * depth2 << "\n";

    return 0;
}
