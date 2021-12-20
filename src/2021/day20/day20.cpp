#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <set>
#include <string>

namespace aoc
{

using namespace std;

class Grid
{
public:

    Grid(const string& algorithm, bool infiniteValue = false)
     : myAlgorithm(algorithm)
     , myInfiniteValue(infiniteValue)
    {}
    Grid(const Grid& other) = delete;
    Grid(Grid&& other) noexcept
    : myData(exchange(other.myData, {}))
    , myAlgorithm(exchange(other.myAlgorithm, {}))
    , myMin(exchange(other.myMin, {}))
    , myMax(exchange(other.myMax, {}))
    , myInfiniteValue(exchange(other.myInfiniteValue, {}))
     {}

    Grid& operator=(Grid&& other) noexcept
    {
        if (this != &other)
        {
            myData = exchange(other.myData, {});
            myAlgorithm = exchange(other.myAlgorithm, {});
            myMin = exchange(other.myMin, {});
            myMax = exchange(other.myMax, {});
            myInfiniteValue = exchange(other.myInfiniteValue, {});
        }

        return *this;
    }

    void put(int16_t x, int16_t y)
    {
        auto [it, result]  = myData.insert(key(x, y));

        if (result)
        {
            myMin[0] = min<int16_t>(myMin[0], x);
            myMin[1] = min<int16_t>(myMin[1], y);
            myMax[0] = max<int16_t>(myMax[0], x);
            myMax[1] = max<int16_t>(myMax[1], y);
        }
    }

    bool get(int16_t x, int16_t y) const
    {
        if (x < myMin[0] || x > myMax[0] || y < myMin[1] || y > myMax[1])
            return myInfiniteValue;

        if (myData.find(key(x, y)) != myData.cend())
            return true;
        
        return false;
    }

    uint32_t count() const
    {
        uint32_t count = 0;

        for (auto yIt = myMin[1]; yIt <= myMax[1]; ++yIt)
            for (auto xIt = myMin[0]; xIt <= myMax[0]; ++xIt)
                count += get(xIt, yIt);

        return count;
    }

    uint16_t code(int16_t x, int16_t y) const
    {
        uint8_t count = 9;
        uint16_t code = 0;

        for (int16_t yIt = y-1; yIt <= y+1; ++yIt)
            for (int16_t xIt = x-1; xIt <= x+1; ++xIt)
                code |= get(xIt, yIt) << --count;

        return code;
    }

    void cycle()
    {
        Grid next(myAlgorithm, myAlgorithm[myInfiniteValue ? 0b111111111 : 0] == '#');

        for (auto yIt = myMin[1]-1; yIt <= myMax[1]+1; ++yIt)
            for (auto xIt = myMin[0]-1; xIt <= myMax[0]+1; ++xIt)
                if (myAlgorithm[code(xIt, yIt)] == '#')
                    next.put(xIt, yIt);

        *this = move(next);
    }

    void print() const
    {
        cout << "\nSize: (" << myMax[0] - myMin[0] << ',' << myMax[1] - myMin[1] << ')';
        
        // for (auto y = myMin[1] - 3; y <= myMax[1] + 3; ++y, cout << '\n')
        //     for (auto x = myMin[0] - 3; x <= myMax[0] + 3; ++x)
        //         cout << (get(x, y) ? '#' : '.');

        cout << "\nCount: " << count();
    }

private:

    static uint32_t key(int16_t x, int16_t y)
    {
        uint32_t yKey = (static_cast<uint32_t>(y) + (1 << 15)) << 16;
        uint32_t xKey = static_cast<uint32_t>(x) + (1 << 15);
        return yKey | xKey;
    }

    set<uint32_t> myData;
    string myAlgorithm;
    array<int16_t, 2> myMin = { 0, 0 };
    array<int16_t, 2> myMax = { -1, -1 };
    bool myInfiniteValue = false;
};

}

int main()
{
    using namespace aoc;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    unique_ptr<Grid> image;
    string algorithm;
    string line;
    while (getline(inputFile, line, '\n'))
    {
        algorithm = line;

        image = make_unique<Grid>(algorithm);

        if (inputFile.peek() == '\n')
        {
            inputFile.get();
            string line2;

            int y = 0;
            while (getline(inputFile, line2, '\n'))
            {
                int x = 0;
                for (auto c : line2)
                {
                    if (c == '#')
                        image->put(x, y);

                    x++;
                }
                y++;
            }

            break;
        }
    }

    image->print();

    for (unsigned i = 0; i < 50; i++)
    {
        image->cycle();
        image->print();
    }

    return 0;
}
