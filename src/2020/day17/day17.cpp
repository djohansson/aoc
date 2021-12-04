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

    void set(int16_t x, int16_t y, int16_t z)
    {
        using namespace std;
        
        auto [it, result]  = myData.insert(key(x, y, z));

        if (result)
        {
            myMin[0] = min(myMin[0], x);
            myMin[1] = min(myMin[1], y);
            myMin[2] = min(myMin[2], z);

            myMax[0] = max(myMax[0], x);
            myMax[1] = max(myMax[1], y);
            myMax[2] = max(myMax[2], z);
        }
    }

    bool get(int16_t x, int16_t y, int16_t z) const
    {
        if (myData.find(key(x, y, z)) != myData.cend())
            return true;
        
        return false;
    }

    uint32_t count() const
    {
        uint32_t count = 0;

        for (auto z = myMin[2]; z <= myMax[2]; ++z)
            for (auto y = myMin[1]; y <= myMax[1]; ++y)
                for (auto x = myMin[0]; x <= myMax[0]; ++x)
                    count += get(x, y, z);

        return count;
    }

    uint8_t neighbourCount(int16_t x, int16_t y, int16_t z) const
    {
        uint8_t count = 0;

        for (auto zIt = z-1; zIt <= z+1; ++zIt)
            for (auto yIt = y-1; yIt <= y+1; ++yIt)
                for (auto xIt = x-1; xIt <= x+1; ++xIt)
                    count += (zIt == z && yIt == y && xIt == x ? 0 : get(xIt, yIt, zIt));

        return count;
    }

    void cycle()
    {
        Grid next;

        for (auto z = myMin[2]-1; z <= myMax[2]+1; ++z)
        {
            for (auto y = myMin[1]-1; y <= myMax[1]+1; ++y)
            {
                for (auto x = myMin[0]-1; x <= myMax[0]+1; ++x)
                {
                    auto nCount = neighbourCount(x, y, z);

                    if (get(x, y, z))
                    {
                        if (nCount >= 2 && nCount <= 3)
                        {
                            next.set(x, y, z);
                        }
                    }
                    else if (nCount == 3)
                    {
                        next.set(x, y, z);
                    }
                }
            }
        }

        *this = std::move(next);
    }

    void print() const
    {
        for (auto z = myMin[2]; z <= myMax[2]; ++z)
        {
            std::cout << "z=" << z << "\n";
            for (auto y = myMin[1]; y <= myMax[1]; ++y)
            {
                for (auto x = myMin[0]; x <= myMax[0]; ++x)
                    std::cout << (get(x, y, z) ? "#" : ".");
                
                std::cout << "\n";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

private:
    static uint64_t key(int16_t x, int16_t y, int16_t z)
    {
        uint64_t zKey = (static_cast<uint64_t>(z) + (1 << 15)) << 32;
        uint64_t yKey = (static_cast<uint64_t>(y) + (1 << 15)) << 16;
        uint64_t xKey = static_cast<uint64_t>(x) + (1 << 15);
        uint64_t key = zKey | yKey | xKey;
        return key;
    }

    std::set<uint64_t> myData;
    std::array<int16_t, 3> myMin = { 0, 0, 0 };
    std::array<int16_t, 3> myMax = { -1, -1, -1 };
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
                    grid.set(x, y, 0);
                    break;
                default:
                    break;
                }

                ++x;
            }

            ++y;
        }
    }

    grid.print();

    for (unsigned cycle = 0; cycle < 6; cycle++)
    {
        cout << "cycle " << cycle << "\n\n";

        grid.cycle();
        grid.print();
    }

    cout << grid.count() << "\n";

    return 0;
}