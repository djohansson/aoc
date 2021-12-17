#include <algorithm>
#include <cassert>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

namespace aoc
{

using namespace std;

static unsigned traverse(const vector<bool>& b, vector<bool>::const_iterator& bIt)
{
    cout << '\n';

    auto bBegin = b.begin();
    auto bEnd = b.end();
    auto nextBit = distance(bBegin, bIt);

    constexpr unsigned cx_versionSize = 3;
    unsigned version = 0;
    nextBit += cx_versionSize;
    for (unsigned i = cx_versionSize - 1; bIt != next(bBegin, nextBit); ++bIt, --i)
    {
        version |= *bIt << i;
        cout << *bIt;
    }

    cout << "\nversion: " << static_cast<unsigned>(version) << '\n';

    constexpr unsigned cx_typeIdSize = 3;
    uint8_t typeId = 0;
    nextBit += cx_typeIdSize;
    for (unsigned i = cx_typeIdSize - 1; bIt != next(bBegin, nextBit); ++bIt, --i)
    {
        typeId |= *bIt << i;
        cout << *bIt;
    }

    //cout << "typeId: " << static_cast<unsigned>(typeId);

    cout << '|';

    if (typeId == 4)
    {
        constexpr unsigned cx_literalSize = 5;
        bool isLast = false;
        do
        {
            cout << *bIt;
            isLast = !(*bIt++);
            unsigned literal = 0;
            nextBit += cx_literalSize;
            for (unsigned i = cx_literalSize - 2; bIt != next(bBegin, nextBit); ++bIt, --i)
            {
                literal |= *bIt << i;
                cout << *bIt;
            }
            cout << "(literal:" << literal << ')';
            
            if (!isLast)
                cout << '|';

        } while (!isLast);

        return version;
    }

    constexpr unsigned cx_lengthTypeIdSize = 1;
    uint8_t lengthTypeId = 0;
    nextBit += cx_lengthTypeIdSize;
    for (unsigned i = cx_lengthTypeIdSize - 1; bIt != next(bBegin, nextBit); ++bIt, --i)
    {
        lengthTypeId |= *bIt << i;
        cout << *bIt;
    }

    cout << '|';

    if (lengthTypeId)
    {
        constexpr unsigned cx_lengthBitsSize = 11;
        unsigned subPacketCount = 0;
        nextBit += cx_lengthBitsSize;
        for (unsigned i = cx_lengthBitsSize - 1; bIt != next(bBegin, nextBit); ++bIt, --i)
        {
            subPacketCount |= *bIt << i;
            cout << *bIt;
        }

        cout << '|';

        while (bIt != bEnd && subPacketCount)
        {
            version += traverse(b, bIt);
            nextBit = distance(bBegin, bIt);
            subPacketCount--;
        }

        assert(subPacketCount == 0);
    }
    else
    {
        constexpr unsigned cx_lengthBitsSize = 15;
        unsigned lengthBits = 0;
        nextBit += cx_lengthBitsSize;
        for (unsigned i = cx_lengthBitsSize - 1; bIt != next(bBegin, nextBit); ++bIt, --i)
        {
            lengthBits |= *bIt << i;
            cout << *bIt;
        }

        cout << '|';

        auto bStart = bIt;
        nextBit += lengthBits;
        while (distance(bStart, bIt) < lengthBits)
            version += traverse(b, bIt);

        assert(distance(bStart, bIt) == lengthBits);
        (void)bStart;
        (void)lengthBits;
    }

    return version;
}

}

int main()
{
    using namespace aoc;

    vector<string> encodings;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    unsigned total = 0;
    string line;
    while (getline(inputFile, line, '\n'))
    {
        vector<bool> packet;
        for (auto c : line)
        {
            int value = (c >= 'A') ? (c - 'A' + 10) : (c - '0');
            cout << format("{:X}", value);
            for (int i = 3; i >= 0; --i)
                packet.push_back(value >> i & 0b1);
        }

        cout << '\n';

        for (auto b : packet)
            cout << b;

        auto packetIt = begin(packet);
        auto versionSum = traverse(packet, packetIt);
        total += versionSum;

        cout << '\n';

        cout << "version sum: " << versionSum << '\n';
    }

    cout << "total: " << total << '\n';
    
    return 0;
}