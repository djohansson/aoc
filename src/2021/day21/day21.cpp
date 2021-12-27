#include <cassert>
#include <cstdint>
#include <iostream>
//#include <unordered_set>

#include <robin_hood.h>

namespace aoc
{

using namespace robin_hood;

template <typename T>
constexpr auto sizeof_array(const T& iarray)
{
    return (sizeof(iarray) / sizeof(iarray[0]));
}

constexpr unsigned cx_starts[] = { 4, 8 };
constexpr unsigned cx_outcomes[][2] =
{
    { 3, 1 }, //{ 1, 1, 1 },
    { 4, 3 }, //{ 1, 1, 2 },{ 1, 2, 1 },{ 2, 1, 1 },
    { 5, 6 }, //{ 1, 1, 3 },{ 1, 2, 2 },{ 1, 3, 1 },{ 2, 1, 2 },{ 2, 2, 1 },{ 3, 1, 1 },
    { 6, 7 }, //{ 1, 2, 3 },{ 1, 3, 2 },{ 2, 1, 3 },{ 2, 2, 2 },{ 2, 3, 1 },{ 3, 1, 2 },{ 3, 2, 1 },
    { 7, 6 }, //{ 1, 3, 3 },{ 2, 2, 3 },{ 2, 3, 2 },{ 3, 1, 3 },{ 3, 2, 2 },{ 3, 3, 1 },
    { 8, 3 }, //{ 2, 3, 3 },{ 3, 2, 3 },{ 3, 3, 2 },
    { 9, 1 }  //{ 3, 3, 3 }
};

struct GameState
{
    union
    {
        struct Player
        {
            uint32_t score : 5;
            uint32_t count : 22;
            uint32_t position : 5;
        } player[2];
        uint64_t raw;
    };
    GameState(uint8_t p1pos, uint8_t p2pos)
    : player{ {0, 1, p1pos}, {0, 1, p2pos} }
    { }
    GameState(const GameState& other)
    : raw(other.raw)
    { }
    inline GameState& operator=(const GameState& other) { raw = other.raw; return *this; }
    inline bool operator==(const GameState& other) const { return raw == other.raw; }
    //inline bool operator<(const GameState& other) const { return raw < other.raw; }
};

}

namespace std
{

template<>
struct hash<aoc::GameState>
{
    inline size_t operator()(const aoc::GameState& p) const noexcept
    {
        return p.raw;
    }
};

}

int main()
{
    using namespace aoc;

    unordered_set<GameState> gs[2];

    using namespace std;

    //set<GameState> gs[2];
    
    gs[0].emplace(GameState(uint8_t(cx_starts[0] - 1), uint8_t(cx_starts[1] - 1)));

    uint64_t rounds = 0;
    //uint8_t die = 0;
    uint64_t wins[2] = { 0ull, 0ull };

    auto gIt = gs[0].begin();
    while (gIt != gs[0].end())
    {
        auto& g = *gIt;

        for (auto p = 0; p < 2; p++)
        {
            for (auto o = 0; o < sizeof_array(cx_outcomes); o++)
            {
                auto gp = g;
                gp.player[p].position += cx_outcomes[o][0];
                gp.player[p].position %= 10;
                gp.player[p].score += (gp.player[p].position + 1);
                gp.player[p].count *= cx_outcomes[o][1];
                if (gp.player[p].score >= 21)
                {
                    wins[p] += gp.player[p].count;

                    // cout << "p" << p << " wins: " << wins[p] << 
                    //     ", score: " << gp.player[p].score << ", count: " << gp.player[p].count << '\n';
                }
                else
                {
                    gs[1].emplace(move(gp));
                }
            }
        }

        if (++gIt == gs[0].end())
        {
            gs[0].clear();
            swap(gs[0], gs[1]);
            gIt = gs[0].begin();

            cout << "round: " << ++rounds << ", set size: " << gs[0].size() << '\n';
        }
    }

    cout << wins[0] << '\n';
    cout << wins[1] << '\n';

    return 0;
}
