#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
//#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

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
static constexpr Coord<T, N> cx_lowestCoord = make_array_n<N>(numeric_limits<T>::lowest());

template<typename T, uint8_t N>
static constexpr Coord<T, N> cx_minCoord = make_array_n<N>(numeric_limits<T>::min());

template<typename T, uint8_t N>
static constexpr Coord<T, N> cx_maxCoord = make_array_n<N>(numeric_limits<T>::max());

template <class T>
static void hash_combine(size_t& seed, const T& v)
{
    hash<T> hasher;
    constexpr size_t cx_kMul = 0x9ddfea08eb382d69ull;
    size_t a = (hasher(v) ^ seed) * cx_kMul;
    a ^= (a >> 47);
    size_t b = (seed ^ a) * cx_kMul;
    b ^= (b >> 47);
    seed = b * cx_kMul;
}

template<typename T, uint8_t N>
struct CoordHash
{
    size_t operator()(const Coord<T, N>& coord) const
    {
        size_t result = 0;

        for (auto c : coord)
            hash_combine(result, c);

        return result;
    }
};

template<typename T, uint8_t N>
class BoundsIterator;
template <typename T, uint8_t N>
class Bounds
{
public:

    static constexpr size_t cx_cornerCount = 1 << N;

    using value_type = tuple<Coord<T, N>, Coord<T, N>>;
	
    constexpr Bounds() noexcept
		: myData{cx_maxCoord<T, N>, cx_lowestCoord<T, N>}
    { }

    constexpr Bounds(const Bounds<T, N>& other) noexcept
		: myData{other.myData}
	{ }
    
    template <typename U = T>
	constexpr Bounds(const Bounds<U, N>& other) noexcept
		: myData{static_cast<value_type>(other.myData)}
	{ }

    constexpr Bounds(const Coord<T, N>& min, const Coord<T, N>& max) noexcept
		: myData{min, max}
    { }

    constexpr Bounds(Coord<T, N>&& min, Coord<T, N>&& max) noexcept
		: myData{forward<Coord<T, N>>(min), forward<Coord<T, N>>(max)}
    { }

    Bounds& operator=(const Bounds& other)
    {
        if (this != &other)
            myData = other.myData;

        return *this;
    }

    Bounds& operator=(Bounds&& other)
    {
        if (this != &other)
            myData = forward<Bounds>(other.myData);
        
        return *this;
    }

	operator bool() const
	{
		return (max() < min()) == 0;
	}

	bool operator!() const
	{
		return !static_cast<bool>(*this);
	}

    BoundsIterator<T, N> begin() const
    {
        return BoundsIterator<T, N>(*this);
    }

    BoundsIterator<T, N> end() const
    {
        return BoundsIterator<T, N>();
    }

    auto& min() { return get<0>(myData); };
    const auto& min() const { return get<0>(myData); };

    auto& max() { return get<1>(myData); };
    const auto& max() const { return get<1>(myData); };

    auto& data() { return myData; };
    const auto& data() const { return myData; };

private:

    value_type myData;
};

template<typename T, uint8_t N>
static constexpr Bounds<T, N> cx_invalidBounds = {};

template<typename T, uint8_t N>
class BoundsIterator
{
public:

    using iterator_category = forward_iterator_tag;
    using value_type = Coord<T, N>;
    using difference_type = value_type;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    constexpr BoundsIterator() noexcept
    : myBounds(cx_invalidBounds<T, N>)
    , myPos(cx_lowestCoord<T, N>)
    { }

    constexpr BoundsIterator(const Bounds<T, N>& bounds) noexcept
    : myBounds(bounds)
    , myPos(myBounds.min())
    { }

    constexpr BoundsIterator(Bounds<T, N>&& bounds) noexcept
    : myBounds(forward<Bounds<T, N>>(bounds))
    , myPos(myBounds.min())
    { }

    constexpr BoundsIterator(const Bounds<T, N>& bounds, const Coord<T, N>& start) noexcept
    : myBounds(bounds)
    , myPos(start)
    { }

    constexpr BoundsIterator(Bounds<T, N>&& bounds, Coord<T, N>&& start) noexcept
    : myBounds(forward<Bounds<T, N>>(bounds))
    , myPos(forward<Coord<T, N>>(start))
    { }

    constexpr BoundsIterator(const BoundsIterator& other) noexcept
    : myBounds(other.myBounds)
    , myPos(other.myPos)
    { }
    
    constexpr BoundsIterator(BoundsIterator&& other) noexcept
    : myBounds(forward<Bounds<T, N>>(other.myBounds))
    , myPos(forward<Coord<T, N>>(other.myPos))
    { }

    BoundsIterator& operator=(const BoundsIterator& other)
    {
        if (this != &other)
        {
            myBounds = other.myBounds;
            myPos = other.myPos;
        }

        return *this;
    }

    BoundsIterator& operator=(BoundsIterator&& other)
    {
        if (this != &other)
        {
            myBounds = forward<Bounds<T, N>>(other.myBounds);
            myPos = forward<Coord<T, N>>(other.myPos);
        }

        return *this;
    }

    operator bool() const
	{
		return valid();
	}

	bool operator!() const
	{
		return !static_cast<bool>(*this);
	}

    bool operator==(const BoundsIterator& other) const
    {
        return myBounds == other.myBounds && myPos == other.myPos;
    }

    bool operator!=(const BoundsIterator& other) const
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

    BoundsIterator operator++(int)
    {
        auto result(*this);

        ++(*this);
        
        return result;
    }

    BoundsIterator& operator++()
    {
        if (!valid())
            return *this;

        const auto& [bmin, bmax] = myBounds.data();

        if (myPos == bmax)
        {
            myBounds = cx_invalidBounds<T, N>;
            myPos = cx_lowestCoord<T, N>;
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
        const auto& [bmin, bmax] = myBounds.data();

        if (myPos > bmax || myPos < bmin)
            return false;

        return true;
    }

private:

    Bounds<T, N> myBounds;
    Coord<T, N> myPos;
};


template<typename T, uint8_t N>
class Grid : public Bounds<T, N>
{
public:

    Grid()
    : Bounds<T, N>(cx_minCoord<T, N>, cx_maxCoord<T, N>)
    { }
    
    Grid(const Grid& other)
    : Bounds<T, N>(other)
    , myData(other.data)
    { }
    
    Grid(Grid&& other) noexcept
    : Bounds<T, N>(forward<Bounds<T, N>>(other))
    , myData(exchange(other.myData, {}))
    { }

    Grid& operator=(const Grid& other) noexcept
    {
        if (this != &other)
        {
            static_cast<Bounds<T, N>>(*this) = other;
            myData = other.myData;
        }

        return *this;
    }

    Grid& operator=(Grid&& other) noexcept
    {
        if (this != &other)
        {
            static_cast<Bounds<T, N>>(*this) = other;
            myData = exchange(other.myData, {});
        }

        return *this;
    }

    template<typename ...Ts>
    void put(Ts... args) { put(Coord<T, sizeof...(Ts)>{args...}); }
    void put(const Coord<T, N>& coord) { myData.emplace(coord); }
    void put(const Bounds<T, N>& bounds) { for (const auto& c : bounds) put(c); }

    template<typename ...Ts>
    void clear(Ts... args) { clear(Coord<T, sizeof...(Ts)>{args...}); }
    void clear(const Coord<T, N>& coord) { myData.erase(coord); }
    void clear(const Bounds<T, N>& bounds) { for (const auto& c : bounds) clear(c); }

    template<typename ...Ts>
    bool get(Ts... args) const { return get(Coord<T, sizeof...(Ts)>{args...}); }
    bool get(const Coord<T, N>& coord) const { return myData.contains(coord); }

    auto size() const { return myData.size(); } const
    auto size(const Bounds<T, N>& bounds) const
    {
        size_t result = 0;

        for (const auto& c : bounds)
            result += get(c);

        return result;
    }

    auto print()
    {
        cout << "load factor: " << myData.load_factor() << '\n';
        // for (const auto& coord : myData)
        // {
        //     for (auto c : coord)
        //         cout << c << ',';

        //     cout << '\b';
        //     cout << ' ';
        //     cout << '\n';
        // }
    }

private:

    // static uint64_t key(const Coord<T, N>& coord)
    // {
    //     constexpr size_t cx_valueBitCount = sizeof(T) * 8;
    //     uint64_t result = 0;

    //     for (size_t i = 0; i < coord.size(); i++)
    //         result |= (static_cast<uint64_t>(coord[i]) + (1 << (cx_valueBitCount - 1))) << (i * cx_valueBitCount);
        
    //     return result;
    // }

    //set<Coord<T, N>> myData;
    robin_hood::unordered_set<Coord<T, N>, CoordHash<T, N>> myData;
};

enum class Operation { Fill, Clear };

template<typename T, uint8_t N>
using Command = tuple<Operation, Bounds<T, N>>;

}

int main()
{
    using namespace aoc;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    Grid<int32_t, 3> grid;
    vector<Command<int32_t, 3>> commands;
    constexpr auto cx_initRegion = Bounds<int32_t, 3>{{-50, -50, -50}, {50, 50, 50}};

    string line;
    while (getline(inputFile, line, '\n'))
    {
        auto& [op, bounds] = commands.emplace_back();
        auto& [bmin, bmax] = bounds.data();
        auto commandStr = split(line, ' ');
        if (commandStr.front() == "on")
            op = Operation::Fill;
        else if (commandStr.front() == "off")
            op = Operation::Clear;

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
        const auto& [op, bounds] = command;
        const auto& [bmin, bmax] = bounds.data();
        const auto& [wmin, wmax] = cx_initRegion.data();
        if (bmin < wmin || bmax > wmax)
            continue;

        if (op == Operation::Fill)
            grid.put(bounds);
        else if (op == Operation::Clear)
            grid.clear(bounds);
        else
            return -1;

        cout << "grid size: " << grid.size() << '\n';
        cout << "init region count: " << grid.size(cx_initRegion) << '\n';
        grid.print();
    }

    return 0;
}
