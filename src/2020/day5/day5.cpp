#pragma once

#include <fstream>
#include <iostream>
#include <string>

int main()
{
    using namespace std;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    int maxSeatId = 0;

    std::string pass;
    while (getline(inputFile, pass))
    {
    //static const string passes[] = { "FBFBBFFRLR", "BFFFBBFRRR", "FFFBBBFRRR", "BBFFBBFRLL" };
    //for (const auto& pass : passes)
    //{
        int row = 0, rowBit = 1 << 6, col = 0, colBit = 1 << 2;

        for (char c : pass)
        {
            switch (c)
            {
            case 'B':
                row |= rowBit;
                [[fallthrough]];
            case 'F':
                rowBit >>= 1;
                break;
            case 'R':
                col |= colBit;
                [[fallthrough]];
            case 'L':
                colBit >>= 1;
                break;
            default:
                return -1;
            }
        }

        int seatID = row * 8 + col;

        cout << "pass: " << pass << "\n";
        cout << "row: " << row << "\n";
        cout << "col: " << col << "\n";
        cout << "seatID: " << seatID << "\n";

        maxSeatId = max(seatID, maxSeatId);
    }

    cout << "maxSeatId: " << maxSeatId << "\n";

    return 0;
}
