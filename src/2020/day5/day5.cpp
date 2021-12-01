#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    using namespace std;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    unsigned maxSeatId = 0;
    vector<unsigned> seatIds;

    string pass;
    while (getline(inputFile, pass))
    {
    //static const string passes[] = { "FBFBBFFRLR", "BFFFBBFRRR", "FFFBBBFRRR", "BBFFBBFRLL" };
    //for (const auto& pass : passes)
    //{
        unsigned row = 0, rowBit = 1 << 6, col = 0, colBit = 1 << 2;

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

        unsigned seatID = row * 8 + col;

        // cout << "pass: " << pass << "\n";
        // cout << "row: " << row << "\n";
        // cout << "col: " << col << "\n";
        // cout << "seatID: " << seatID << "\n";

        maxSeatId = max(seatID, maxSeatId);

        seatIds.emplace_back(seatID);
    }

    cout << "maxSeatId: " << maxSeatId << "\n";

    sort(begin(seatIds), end(seatIds));

    for (unsigned seatIt = 1; seatIt < seatIds.size(); seatIt++)
    {
        auto seatId = seatIds[seatIt];
        if (seatIds[seatIt-1] + 1 != seatId)
        {
            cout << "seatId: " << seatIds[seatIt-1] + 1 << "\n";
            return 0;
        }
    }

    return -1;
}
