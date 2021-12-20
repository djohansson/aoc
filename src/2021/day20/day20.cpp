#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <set>

namespace aoc
{

using namespace std;


class Grid
{
public:

    Grid() = default;
    Grid(const Grid& other) = delete;
    Grid(Grid&& other) noexcept
    : myData(exchange(other.myData, {}))
    , myMin(exchange(other.myMin, {}))
    , myMax(exchange(other.myMax, {})) {}

    Grid& operator=(Grid&& other) noexcept
    {
        if (this != &other)
        {
            myData = exchange(other.myData, {});
            myMin = exchange(other.myMin, {});
            myMax = exchange(other.myMax, {});
        }

        return *this;
    }

    void set(int32_t x, int32_t y)
    {
        using namespace std;
        
        auto [it, result]  = myData.insert(key(x, y));

        if (result)
        {
            myMin[0] = min(myMin[0], x);
            myMin[1] = min(myMin[1], y);
            myMax[0] = max(myMax[0], x);
            myMax[1] = max(myMax[1], y);
        }
    }

    bool get(int32_t x, int32_t y) const
    {
        if (myData.find(key(x, y)) != myData.cend())
            return true;
        
        return false;
    }

    uint64_t count() const
    {
        uint64_t count = 0;

        for (auto y = myMin[1]; y <= myMax[1]; ++y)
            for (auto x = myMin[0]; x <= myMax[0]; ++x)
                count += get(x, y);

        return count;
    }

    // uint8_t neighbourCount(int16_t x, int16_t y) const
    // {
    //     uint8_t count = 0;

    //     for (auto yIt = y-1; yIt <= y+1; ++yIt)
    //         for (auto xIt = x-1; xIt <= x+1; ++xIt)
    //             count += (yIt == y && xIt == x ? 0 : get(xIt, yIt));

    //     return count;
    // }

    uint16_t code(int32_t x, int32_t y) const
    {
        uint8_t count = 8;
        uint16_t code = 0;

        for (auto yIt = y-1; yIt <= y+1; ++yIt)
            for (auto xIt = x-1; xIt <= x+1; ++xIt, --count)
                code |= get(xIt, yIt) << count;

        return code;
    }

    void cycle(const string& algorithm)
    {
        Grid next;

        for (auto y = myMin[1]-1; y <= myMax[1]+1; ++y)
            for (auto x = myMin[0]-1; x <= myMax[0]+1; ++x)
                if (algorithm[code(x, y)] == '#')
                    next.set(x, y);
                    
        *this = move(next);
    }

    void print() const
    {
        for (auto y = myMin[1]; y <= myMax[1]; ++y)
        {
            for (auto x = myMin[0]; x <= myMax[0]; ++x)
                cout << (get(x, y) ? '#' : '.');
            
            cout << '\n';
        }
        cout << '\n';
    }

private:
    static uint64_t key(int32_t x, int32_t y)
    {
        uint64_t yKey = (static_cast<uint64_t>(y) + (1 << 15)) << 16;
        uint64_t xKey = static_cast<uint64_t>(x) + (1 << 15);
        return yKey | xKey;
    }

    std::set<uint64_t> myData;
    array<int32_t, 2> myMin = { 0, 0 };
    array<int32_t, 2> myMax = { -1, -1 };
};

}

int main()
{
    using namespace aoc;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    Grid image;
    string algorithm;
    string line;
    while (getline(inputFile, line, '\n'))
    {
        algorithm = line;

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
                        image.set(x, y);

                    x++;
                }
                y++;
            }

            break;
        }
    }

    image.print();
    image.cycle(algorithm);
    //image.print();
    image.cycle(algorithm);
    //image.print();

    cout << image.count();

    return 0;
}
