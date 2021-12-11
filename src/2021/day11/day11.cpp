
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <fstream>
#include <string>
#include <vector>

namespace aoc
{

using namespace std;
using Levels = vector<uint32_t>;

constexpr uint32_t cx_flashBit = 1 << 31;
constexpr uint32_t cx_border = ~0u & ~cx_flashBit;

static inline unsigned index(uint16_t x, uint16_t y, uint16_t rowSize)
{
    return y * rowSize + x;
}

template <typename T>
static inline void flash(uint16_t x, uint16_t y, uint16_t rowSize, T& data, uint32_t& count)
{
    auto& val = data[index(x, y, rowSize)];

    if (val == cx_border)
        return;

    if (val & cx_flashBit)
        return;

    if (val > 9)
    {
        val = cx_flashBit;
        count += 1;

        for (auto yIt = y - 1; yIt <= y + 1; ++yIt)
            for (auto xIt = x - 1; xIt <= x + 1; ++xIt)
                if (xIt != x || yIt != y)
                {
                    auto& val2 = data[index(xIt, yIt, rowSize)];
                    
                    if (val2 == cx_border)
                        continue;

                    if (val2 & cx_flashBit)
                        continue;

                    ++val2;
                        
                    flash(xIt, yIt, rowSize, data, count);
                }
                    
    }
}

}

int main()
{
    using namespace aoc;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    unsigned colSize = 0;
    unsigned rowSize = 0;

    Levels current;

    string line;
    while (getline(inputFile, line, '\n'))
    {
        rowSize = max(unsigned(line.size()) + 2, rowSize);
        current.resize(current.size() + rowSize);
        
        if (colSize++ == 0)
        {
            fill(begin(current), end(current), cx_border);
            current.resize(current.size() + rowSize);
        }

        generate_n(end(current) - rowSize + 1, line.size(), [cIt = line.begin()]() mutable { return static_cast<unsigned>(*cIt++) - 48u; });
        *(end(current) - 1) = *(end(current) - rowSize) = cx_border;
        
        if (inputFile.peek() == char_traits<char>::eof())
        {
            colSize += 2;
            current.resize(current.size() + rowSize);
            fill(end(current) - rowSize, end(current), cx_border);
        }
    }

    auto print = [rowSize](const auto& data)
    {
        for_each(begin(data), end(data), [idx = 0, rowSize](auto level) mutable
        {
            cout << std::right << setw(2);
            
            if (level == cx_border)
                cout << '#';
            else
                cout << level;
            
            if (++idx % rowSize == 0)
                cout << '\n';
        });

        cout << '\n';
    };

    auto step = [rowSize, colSize](auto& data)
    {
        uint32_t count = 0;

        for_each(begin(data), end(data), [](auto& level) { level = (level == cx_border ? cx_border : level + 1); });

        for (uint16_t yIt = 1; yIt < colSize - 1; ++yIt)
            for (uint16_t xIt = 1; xIt < rowSize - 1; ++xIt)
                flash(xIt, yIt, rowSize, data, count);

        for_each(begin(data), end(data), [](auto& level) { level &= ~cx_flashBit; });

        return count;
    };

    cout << "Before any steps:" << "\n";
    print(current);

    uint32_t flashCount = 0;
    uint32_t firstSync = 0;

    for (uint32_t i = 1; i <= 500; i++)
    {
        //cout << "After step: " << i << "\n";
        flashCount += step(current);
        //print(current);

        if (!firstSync)
        {
            bool allSame = true;
            uint32_t val = current[index(1, 1, rowSize)];
            for (uint16_t yIt = 1; yIt < colSize - 1; ++yIt)
                for (uint16_t xIt = 1; xIt < rowSize - 1; ++xIt)
                    allSame &= (current[index(xIt, yIt, rowSize)] == val);

            if (allSame)
                firstSync = i;
        }
    }

    cout << "Flash count: " << flashCount << "\n";
    cout << "First sync: " << firstSync << "\n";

    return 0;
}
