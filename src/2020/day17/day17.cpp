#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <set>

class Grid
{
public:

    Grid() = default;
    Grid(const Grid& other) = delete;
    Grid(Grid&& other) noexcept
    : myData(std::exchange(other.myData, {}))
    , myMin(std::exchange(other.myMin, {}))
    , myMax(std::exchange(other.myMax, {})) {}

    Grid& operator=(Grid&& other) noexcept
    {
        if (this != &other)
        {
            myData = std::exchange(other.myData, {});
            myMin = std::exchange(other.myMin, {});
            myMax = std::exchange(other.myMax, {});
        }

        return *this;
    }

    void set(int16_t x, int16_t y, int16_t z, int16_t w)
    {
        using namespace std;
        
        auto [it, result]  = myData.insert(key(x, y, z, w));

        if (result)
        {
            myMin[0] = min(myMin[0], x);
            myMin[1] = min(myMin[1], y);
            myMin[2] = min(myMin[2], z);
            myMin[3] = min(myMin[2], w);

            myMax[0] = max(myMax[0], x);
            myMax[1] = max(myMax[1], y);
            myMax[2] = max(myMax[2], z);
            myMax[3] = max(myMax[2], w);
        }
    }

    bool get(int16_t x, int16_t y, int16_t z, int16_t w) const
    {
        if (myData.find(key(x, y, z, w)) != myData.cend())
            return true;
        
        return false;
    }

    uint32_t count() const
    {
        uint32_t count = 0;

        for (auto w = myMin[3]; w <= myMax[3]; ++w)
            for (auto z = myMin[2]; z <= myMax[2]; ++z)
                for (auto y = myMin[1]; y <= myMax[1]; ++y)
                    for (auto x = myMin[0]; x <= myMax[0]; ++x)
                        count += get(x, y, z, w);

        return count;
    }

    uint8_t neighbourCount(int16_t x, int16_t y, int16_t z, int16_t w) const
    {
        uint8_t count = 0;

        for (auto wIt = w-1; wIt <= w+1; ++wIt)
            for (auto zIt = z-1; zIt <= z+1; ++zIt)
                for (auto yIt = y-1; yIt <= y+1; ++yIt)
                    for (auto xIt = x-1; xIt <= x+1; ++xIt)
                        count += (wIt == w && zIt == z && yIt == y && xIt == x ? 0 : get(xIt, yIt, zIt, wIt));

        return count;
    }

    void cycle()
    {
        Grid next;

        for (auto w = myMin[3]-1; w <= myMax[3]+1; ++w)
            for (auto z = myMin[2]-1; z <= myMax[2]+1; ++z)
                for (auto y = myMin[1]-1; y <= myMax[1]+1; ++y)
                    for (auto x = myMin[0]-1; x <= myMax[0]+1; ++x)
                    {
                        auto nCount = neighbourCount(x, y, z, w);

                        if (get(x, y, z, w))
                        {
                            if (nCount >= 2 && nCount <= 3)
                                next.set(x, y, z, w);
                        }
                        else if (nCount == 3)
                        {
                            next.set(x, y, z, w);
                        }
                    }
                    
        *this = std::move(next);
    }

    void print() const
    {
        for (auto w = myMin[3]; w <= myMax[3]; ++w)
        {
            for (auto z = myMin[2]; z <= myMax[2]; ++z)
            {
                std::cout << "z=" << z << ", " << "w=" << w << "\n";
                for (auto y = myMin[1]; y <= myMax[1]; ++y)
                {
                    for (auto x = myMin[0]; x <= myMax[0]; ++x)
                        std::cout << (get(x, y, z, w) ? "#" : ".");
                    
                    std::cout << "\n";
                }
                std::cout << "\n";
            }
        }
        std::cout << "\n";
    }

private:
    static uint64_t key(int16_t x, int16_t y, int16_t z, int16_t w)
    {
        uint64_t wKey = (static_cast<uint64_t>(w) + (1 << 15)) << 48;
        uint64_t zKey = (static_cast<uint64_t>(z) + (1 << 15)) << 32;
        uint64_t yKey = (static_cast<uint64_t>(y) + (1 << 15)) << 16;
        uint64_t xKey = static_cast<uint64_t>(x) + (1 << 15);
        return wKey | zKey | yKey | xKey;
    }

    std::set<uint64_t> myData;
    std::array<int16_t, 4> myMin = { 0, 0, 0, 0};
    std::array<int16_t, 4> myMax = { -1, -1, -1, -1 };
};

int main()
{
    using namespace std;

    Grid grid;
    
    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    {
        int16_t y = -1;
        string line;
        while (getline(inputFile, line, '\n'))
        {
            int16_t x = -1;
            for (char c : line)
            {
                switch (c)
                {
                case '#':
                    grid.set(x, y, 0, 0);
                    break;
                default:
                    break;
                }

                ++x;
            }

            ++y;
        }
    }

    //grid.print();

    for (unsigned cycle = 0; cycle < 6; cycle++)
    {
        cout << "cycle " << cycle << "\n\n";

        grid.cycle();
        //grid.print();
    }

    cout << grid.count() << "\n";

    return 0;
}