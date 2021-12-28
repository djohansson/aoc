#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
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

        const optional<Coord>& operator*() const
        {
            return myPos;
        }

        const optional<Coord>* operator->() const
        {
            return &myPos;
        }

        BoundsIterator& operator++()
        {
            const auto& [bmin, bmax] = myBounds;

            assert(myPos.has_value());

            if (!myPos.has_value())
                return *this;

            assert(myPos.value() >= bmin);
            assert(myPos.value() <= bmax);

            if (bmin == bmax)
            {
                myPos.reset();
                return *this;
            }
            
            size_t d = 0;
            for (; d < myPos.value().size(); d++)
            {
                auto range = bmax[d] - bmin[d];
                auto shiftToZero = myPos.value()[d] - bmin[d];
                auto next = ++shiftToZero;
                auto rest = next % range;
                auto shiftBack = rest + bmin[d];

                myPos.value()[d] = shiftBack;

                if (next >= range)
                    continue;

                break;
            }

            if (myPos.value() <= bmin || myPos.value() >= bmax)
                myPos.reset();

            return *this;
        }

        bool valid() const { return myPos.has_value(); }

    private:

        const Bounds& myBounds;
        optional<Coord> myPos;
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
    void set(Ts... args) { set(Coord{args...}); }
    void set(const Coord& coord) { myData.emplace(coord); }
    void set(const Bounds& bounds)
    {
        BoundsIterator bIt(bounds);
        while (bIt.valid())
        {
            set(bIt->value());
            ++bIt;
        }
    }

    template<typename ...Ts>
    void clear(Ts... args) { clear(Coord{args...}); }
    void clear(const Coord& coord) { myData.erase(coord); }
    void clear(const Bounds& bounds)
    {
        BoundsIterator bIt(bounds);
        while (bIt.valid())
        {
            clear(bIt->value());
            ++bIt;
        }
    }

    void apply(const Command& command)
    {
        const auto& [op, bounds] = command;
        if (op == Operation::Fill)
            set(bounds);
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

private:

    // static uint64_t key(const Coord& coord)
    // {
    //     constexpr size_t cx_valueBitCount = sizeof(T) * 8;
    //     uint64_t result = 0;
    //     for (size_t i = 0; i < coord.size(); i++)
    //         result |= (static_cast<uint64_t>(coord[i]) + (1 << (cx_valueBitCount - 1))) << (i * cx_valueBitCount);
        
    //     return result;
    // }

    std::set<Coord> myData;
};

}

int main()
{
    using namespace aoc;

    ifstream inputFile("test.txt");
    if (!inputFile.is_open())
        return -1;

    using Grid3i16 = Grid<int16_t, 3>;
    Grid3i16 grid;
    vector<Grid3i16::Command> commands;

    string line;
    while (getline(inputFile, line, '\n'))
    {
        auto& [op, bounds] = commands.emplace_back();
        auto& [bmin, bmax] = bounds;
        auto commandStr = split(line, ' ');
        if (commandStr.front() == "on")
            op = Grid3i16::Operation::Fill;
        else if (commandStr.front() == "off")
            op = Grid3i16::Operation::Clear;

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
        grid.apply(command);
        cout << "grid size: " << grid.size() << '\n';
    }

    cout.flush();

    return 0;
}
