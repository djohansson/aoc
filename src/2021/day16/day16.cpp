#include <cassert>
#include <cstdint>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

namespace aoc
{

using namespace std;

static uint64_t traverse(const vector<bool>& b, vector<bool>::const_iterator& bIt)
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
    (void)version;
    //cout << "\nversion: " << static_cast<unsigned>(version) << '\n';

    constexpr unsigned cx_typeIdSize = 3;
    uint8_t typeId = 0;
    nextBit += cx_typeIdSize;
    for (unsigned i = cx_typeIdSize - 1; bIt != next(bBegin, nextBit); ++bIt, --i)
    {
        typeId |= *bIt << i;
        cout << *bIt;
    }

    function<uint64_t(uint64_t, uint64_t)> op;
    uint64_t init = 0;

    switch (typeId)
    {
    case 0: // sum
        op = plus<>();
        cout << "|(sum)";
        break;
    case 1: // product
        op = multiplies<>();
        cout << "|(product)";
        init = 1;
        break;
    case 2: // minimum
        op = [](uint64_t a, uint64_t b) { return min(a, b); };
        init = ~0ull;
        cout << "|(minimum)";
        break;
    case 3: // maximum
        op = [](uint64_t a, uint64_t b) { return max(a, b); };
        cout << "|(maximum)";
        break;
    case 4:
    {
        constexpr unsigned cx_literalSize = 5;
        bool isLast = false;
        uint64_t literal = 0;
        unsigned literalSize = 0; 
        do
        {
            literalSize += cx_literalSize;
            isLast = !(*bIt++);
            for (unsigned i = 0; i < 4; i++)
            {
                literal <<= 1;
                literal |= *bIt++;
            }
        } while (!isLast);

        nextBit += literalSize;

        cout << "|(literal:" << literal << ')';

        return literal;
    }
    case 5: // greater than
        op = greater<>();
        cout << "|(greater)";
        break;
    case 6: // less than
        op = less<>();
        cout << "|(less)";
        break;
    case 7: // equal to
        op = equal_to<>();
        cout << "|(equal)";
        break;
    default:
        assert(false);
        break;
    }

    cout << '|';

    constexpr unsigned cx_lengthTypeIdSize = 1;
    uint8_t lengthTypeId = 0;
    nextBit += cx_lengthTypeIdSize;
    for (unsigned i = cx_lengthTypeIdSize - 1; bIt != next(bBegin, nextBit); ++bIt, --i)
    {
        lengthTypeId |= *bIt << i;
        cout << *bIt;
    }

    cout << '|';

    vector<uint64_t> values;
    values.reserve(2);

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
            values.emplace_back(traverse(b, bIt));
            nextBit = distance(bBegin, bIt);
            subPacketCount--;
        }

        assert(subPacketCount == 0);
    }
    else
    {
        constexpr unsigned cx_lengthBitsSize = 15;
        uint16_t lengthBits = 0;
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
            values.emplace_back(traverse(b, bIt));

        assert(distance(bStart, bIt) == lengthBits);
        (void)bStart;
        (void)lengthBits;
    }

    if (typeId > 4)
    {
        assert(values.size() == 2);

        return op(values.front(), values.back());
    }

    //assert(values.size() >= 2);

    return accumulate(begin(values), end(values), init, op);
}

}

int main()
{
    using namespace aoc;

    vector<string> encodings;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

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

        auto packetIt = begin(packet);
        auto value = traverse(packet, packetIt);
        
        cout << "\nvalue: " << value << '\n';
    }
    
    return 0;
}