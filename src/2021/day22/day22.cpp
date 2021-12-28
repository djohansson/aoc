#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>

#include <robin_hood.h>

namespace aoc
{

using namespace std;

static vector<string>& split(const string& s, char delim, vector<string>& elems)
{
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim))
    {
        if (item.length() > 0)
            elems.push_back(item);
    }
    return elems;
}

static vector<string> split(const string& s, char delim)
{
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

template<typename T, uint8_t N>
class Grid
{
public:

    using Coord = array<T, N>;
    using Bounds = tuple<array<T, N>, array<T, N>>;
    enum class Operation { Fill, Clear };
    using Command = tuple<Operation, Bounds>;

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

    template<typename ...Ts>
    void set(Ts... args) { set(Coord{args...}); }
    void set(const Coord& coord)
    {
        auto [it, result]  = myData.emplace(key(coord));
        if (result)
        {
            auto& [bmin, bmax] = myBounds;
            bmin = min(bmin, coord);
            bmax = max(bmax, coord);
        }
    }
    void set(const Command& command)
    {
        
    }

    template<typename ...Ts>
    bool get(Ts... args) const { return get(Coord{args...}); }
    bool get(const Coord& coord)
    {
        if (myData.find(key(args...)) != myData.cend())
            return true;
        
        return false;
    }

    auto size() const { return myData.size(); }

private:

    static uint64_t key(const Coord& coord)
    {
        uint64_t result = 0;
        for (size_t i = 0; i < coord.size(); i++)
            result |= (static_cast<uint64_t>(coord[i]) + (1 << (cx_valueBitCount - 1))) << (i * cx_valueBitCount);
        
        return result;
    }

    robin_hood::unordered_set<uint64_t> myData;
    Bounds myBounds;
    static constexpr size_t cx_valueBitCount = sizeof(T) * 8;
};

}

int main()
{
    using namespace aoc;

    Grid<int16_t, 4> grid;
    
    ifstream inputFile("test.txt");
    if (!inputFile.is_open())
        return -1;

    {
        string line;
        while (getline(inputFile, line, '\n'))
        {
            auto commands = split(line, ' ');
        }
    }

    cout << grid.size() << '\n';

    return 0;
}
