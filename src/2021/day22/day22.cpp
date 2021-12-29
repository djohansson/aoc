#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
//#include <unordered_set>
#include <utility>
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

namespace detail
{

template<size_t Size, typename T, size_t... Indexes>
constexpr auto make_array_n_impl(T&& value, index_sequence<Indexes...>)
{
    return array<decay_t<T>, Size>{ (static_cast<void>(Indexes), value)..., forward<T>(value) };
}

} // namespace detail

template<typename T>
constexpr auto make_array_n(integral_constant<size_t, 0>, T&&)
{
    return array<decay_t<T>, 0>{};
}

template<size_t Size, typename T>
constexpr auto make_array_n(integral_constant<size_t, Size>, T&& value)
{
    return detail::make_array_n_impl<Size>(forward<T>(value), make_index_sequence<Size - 1>{});
}

template<size_t Size, typename T>
constexpr auto make_array_n(T&& value)
{
    return make_array_n(integral_constant<size_t, Size>{}, forward<T>(value));
}

template<typename T, uint8_t N>
using Coord = array<T, N>;

template<typename T, uint8_t N>
using Bounds = tuple<Coord<T, N>, Coord<T, N>>;

template<typename T, uint8_t N>
static constexpr Coord<T, N> cx_invalidCoord = make_array_n<N>(numeric_limits<T>::min());

template<typename T, uint8_t N>
class Grid
{
public:

    enum class Operation { Fill, Clear };
    using Command = tuple<Operation, Bounds<T, N>>;
    class Iterator
    {
    public:
    
        using iterator_category = forward_iterator_tag;
        using value_type = Coord<T, N>;
        using difference_type = value_type;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;

        Iterator(const Bounds<T, N>& bounds)
        : myBounds(bounds)
        , myPos(std::get<0>(myBounds))
        { }

        Iterator(const Bounds<T, N>& bounds, const Coord<T, N>& start)
        : myBounds(bounds)
        , myPos(start)
        { }

        Iterator(const Iterator& other)
        : myBounds(other.bounds)
        , myPos(std::get<0>(myBounds))
        { }
        
        Iterator(Iterator&& other)
        : myBounds(exchange(other.bounds, {}))
        , myPos(std::get<0>(myBounds))
        { }

        Iterator& operator=(const Iterator& other)
        {
            myBounds = other.bounds;
            myPos = std::get<0>(myBounds);
            return *this;
        }

        Iterator& operator=(Iterator&& other)
        {
            myBounds = exchange(other.bounds, {});
            myPos = std::get<0>(myBounds);
            return *this;
        }

        bool operator==(const Iterator& other) const
        {
            return myBounds == other.myBounds && myPos == other.myPos;
        }

        bool operator!=(const Iterator& other) const
        {
            return !(*this == other);
        }

        reference& operator*()
        {
            return myPos;
        }

        const_reference& operator*() const
        {
            return myPos;
        }

        pointer* operator->()
        {
            return &myPos;
        }

        const_pointer* operator->() const
        {
            return &myPos;
        }

        Iterator& operator++()
        {
            if (!valid())
            {
                assert(false);
                return *this;
            }

            const auto& [bmin, bmax] = myBounds;
            if (bmin == bmax)
                return *this;

            if (myPos == bmax)
            {
                myPos = cx_invalidCoord<T, N>;
                return *this;
            }

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

        bool valid() const 
        {
            const auto& [bmin, bmax] = myBounds;
            if (myPos > bmax || myPos < bmin)
                return false;

            return true;
        }

    private:

        const Bounds<T, N>& myBounds;
        Coord<T, N> myPos;
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
    void put(Ts... args) { put(Coord<T, sizeof...(Ts)>{args...}); }
    void put(const Coord<T, N>& coord) { myData.emplace(coord); }
    void put(const Bounds<T, N>& bounds)
    {
        Iterator bIt(bounds);
        while (bIt.valid())
        {
            put(*bIt);
            ++bIt;
        }
    }

    template<typename ...Ts>
    void clear(Ts... args) { clear(Coord<T, sizeof...(Ts)>{args...}); }
    void clear(const Coord<T, N>& coord) { myData.erase(coord); }
    void clear(const Bounds<T, N>& bounds)
    {
        Iterator bIt(bounds);
        while (bIt.valid())
        {
            clear(*bIt);
            ++bIt;
        }
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
    bool get(Ts... args) const { return get(Coord<T, sizeof...(Ts)>{args...}); }
    bool get(const Coord<T, N>& coord)
    {
        if (myData.find(coord) != myData.cend())
            return true;
        
        return false;
    }

    auto size() const { return myData.size(); }
    auto size(const Bounds<T, N>& bounds)
    {
        size_t result = 0;
        Iterator bIt(bounds);
        while (bIt.valid())
        {
            result += get(*bIt);
            ++bIt;
        }
        
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
    //     size_t operator()(const Coord<T, N>& coord) const
    //     {
    //         size_t result = 0;
    //         for (auto c : coord)
    //             result ^= hash<typename Coord<T, N>::value_type>{}(c) + 0x9e3779b9 + (result << 6) + (result >> 2);

    //         return result;
    //     }   
    // };

    // static uint64_t key(const Coord<T, N>& coord)
    // {
    //     constexpr size_t cx_valueBitCount = sizeof(T) * 8;
    //     uint64_t result = 0;
    //     for (size_t i = 0; i < coord.size(); i++)
    //         result |= (static_cast<uint64_t>(coord[i]) + (1 << (cx_valueBitCount - 1))) << (i * cx_valueBitCount);
        
    //     return result;
    // }

    set<Coord<T, N>> myData;
    //robin_hood::unordered_set<Coord<T, N>, CoordHash> myData;
    //unordered_set<Coord, CoordHash> myData;
};

}

int main()
{
    using namespace aoc;

    ifstream inputFile("test3.txt");
    if (!inputFile.is_open())
        return -1;

    using Grid3i32 = Grid<int32_t, 3>;
    Grid3i32 grid;
    vector<Grid3i32::Command> commands;
    constexpr auto cx_initRegion = Bounds<int32_t, 3>{{-50, -50 -50}, {50, 50, 50}};

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
        //grid.print();

        cout << "grid size: " << grid.size() << '\n';
        cout << "init region count: " << grid.size(cx_initRegion) << '\n';
    }

    cout.flush();

    return 0;
}
