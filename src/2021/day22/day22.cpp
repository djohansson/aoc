#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
//#include <unordered_set>
#include <vector>

//#include <robin_hood.h>

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

    class BoundsIterator
    {
    public:
    
        using iterator_category = forward_iterator_tag;
        using value_type = T;
        using difference_type = T;
        using pointer = T*;
        using reference = T&;

        BoundsIterator(const Bounds& bounds)
        : myBounds(bounds)
        , myPos(std::get<0>(myBounds))
        { }

        BoundsIterator(const Bounds& bounds, const Coord& pos)
        : myBounds(bounds)
        , myPos(pos)
        { }

        BoundsIterator(const BoundsIterator& other)
        : myBounds(other.bounds)
        , myPos(std::get<0>(myBounds))
        { }
        
        BoundsIterator(BoundsIterator&& other)
        : myBounds(exchange(other.bounds, {}))
        , myPos(std::get<0>(myBounds))
        { }

        BoundsIterator& operator=(const BoundsIterator& other)
        {
            myBounds = other.bounds;
            myPos = std::get<0>(myBounds);
            return *this;
        }

        BoundsIterator& operator=(BoundsIterator&& other)
        {
            myBounds = exchange(other.bounds, {});
            myPos = std::get<0>(myBounds);
            return *this;
        }

        bool operator==(const BoundsIterator& other) const
        {
            return myBounds == other.myBounds && myPos == other.myPos;
        }

        bool operator!=(const BoundsIterator& other) const
        {
            return !(*this == other);
        }

        const Coord& operator*() const
        {
            return myPos;
        }

        const Coord* operator->() const
        {
            return &myPos;
        }

        BoundsIterator& operator++()
        {
            const auto& [bmin, bmax] = myBounds;

            assert(myPos >= bmin);
            assert(myPos <= bmax);

            if (bmin == bmax)
                return *this;

            size_t d = 0;
            for (; d < myPos.size(); d++)
            {
                auto range = bmax[d] - bmin[d] + 1;
                auto shiftToZero = myPos[d] - bmin[d];
                auto next = ++shiftToZero;
                auto rest = next % range;
                auto shiftBack = rest + bmin[d];

                myPos[d] = shiftBack;

                if (next >= range)
                    continue;

                break;
            }

            return *this;
        }

    private:

        const Bounds& myBounds;
        Coord myPos;
    };

    Grid() = default;
    Grid(const Grid& other) = delete;
    
    Grid(Grid&& other) noexcept
    : myData(exchange(other.myData, {}))
    { }

    Grid& operator=(Grid&& other) noexcept
    {
        if (this != &other)
            myData = exchange(other.myData, {});

        return *this;
    }

    template<typename ...Ts>
    void put(Ts... args) { put(Coord{args...}); }
    void put(const Coord& coord) { myData.emplace(coord); }
    void put(const Bounds& bounds)
    {
        const auto& [bmin, bmax] = bounds;
        BoundsIterator bIt(bounds);
        do
        {
            put(*bIt);
            ++bIt;
        } while (*bIt != bmax);
        put(*bIt);
    }

    template<typename ...Ts>
    void clear(Ts... args) { clear(Coord{args...}); }
    void clear(const Coord& coord) { myData.erase(coord); }
    void clear(const Bounds& bounds)
    {
        const auto& [bmin, bmax] = bounds;
        BoundsIterator bIt(bounds);
        do
        {
            clear(*bIt);
            ++bIt;
        } while (*bIt != bmax);
        clear(*bIt);
    }

    void apply(const Command& command)
    {
        const auto& [op, bounds] = command;
        if (op == Operation::Fill)
            put(bounds);
        else
            clear(bounds);
    }

    template<typename ...Ts>
    bool get(Ts... args) const { return get(Coord{args...}); }
    bool get(const Coord& coord)
    {
        if (myData.find(coord) != myData.cend())
            return true;
        
        return false;
    }

    auto size() const { return myData.size(); }
    auto size(const Bounds& bounds)
    {
        size_t result = 0;
        const auto& [bmin, bmax] = bounds;
        BoundsIterator bIt(bounds);
        do
        {
            result += get(*bIt);
            ++bIt;
        } while (*bIt != bmax);
        result += get(*bIt);
        
        return result;
    }

    auto print()
    {
        for (const auto& coord : myData)
        {
            for (auto c : coord)
                cout << c << ',';

            cout << '\b';
            cout << ' ';
            cout << '\n';
        }
    }

private:

    // struct CoordHash
    // {
    //     size_t operator()(const Coord& coord) const
    //     {
    //         size_t result = 0;
    //         for (auto c : coord)
    //             result ^= hash<typename Coord::value_type>{}(c) + 0x9e3779b9 + (result << 6) + (result >> 2);

    //         return result;
    //     }   
    // };

    // static uint64_t key(const Coord& coord)
    // {
    //     constexpr size_t cx_valueBitCount = sizeof(T) * 8;
    //     uint64_t result = 0;
    //     for (size_t i = 0; i < coord.size(); i++)
    //         result |= (static_cast<uint64_t>(coord[i]) + (1 << (cx_valueBitCount - 1))) << (i * cx_valueBitCount);
        
    //     return result;
    // }

    set<Coord> myData;
    //robin_hood::unordered_set<Coord, CoordHash> myData;
    //unordered_set<Coord, CoordHash> myData;
};

}

int main()
{
    using namespace aoc;

    ifstream inputFile("test.txt");
    if (!inputFile.is_open())
        return -1;

    using Grid3i32 = Grid<int32_t, 3>;
    Grid3i32 grid;
    vector<Grid3i32::Command> commands;
    constexpr auto cx_initRegion = Grid3i32::Bounds{{-50, -50 -50}, {50, 50, 50}};

    string line;
    while (getline(inputFile, line, '\n'))
    {
        auto& [op, bounds] = commands.emplace_back();
        auto& [bmin, bmax] = bounds;
        auto commandStr = split(line, ' ');
        if (commandStr.front() == "on")
            op = Grid3i32::Operation::Fill;
        else if (commandStr.front() == "off")
            op = Grid3i32::Operation::Clear;

        auto boundsStr = split(commandStr.back(), ',');
        unsigned i = 0;
        for (const auto& b : boundsStr)
        {
            auto coordStr = split(b, '=');
            bmin[i] = stoi(coordStr[1].substr(0, coordStr[1].find_first_of('.')));
            bmax[i] = stoi(coordStr[1].substr(coordStr[1].find_last_of('.') + 1));
            i++;
        }
    }

    for (const auto& command : commands)
    {
        auto& [op, bounds] = command;
        auto& [bmin, bmax] = bounds;
        if (any_of(begin(bmin), end(bmin), [](auto n) { return n < -50; }) ||
            any_of(begin(bmax), end(bmax), [](auto n) { return n > 50; }))
            continue;

        grid.apply(command);
        grid.print();

        cout << "grid size: " << grid.size() << '\n';
        cout << "init region count: " << grid.size(cx_initRegion) << '\n';
    }

    cout.flush();

    return 0;
}
