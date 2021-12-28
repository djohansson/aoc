#include <cstdint>
#include <iostream>

namespace aoc
{

using namespace std;

template <typename T>
constexpr auto sizeof_array(const T& iarray)
{
    return (sizeof(iarray) / sizeof(iarray[0]));
}

constexpr unsigned cx_starts[] = { 8, 6 };
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
    struct Player
    {
        uint8_t score;
        uint8_t position;
    } player[2];
    
    GameState(uint8_t p1pos, uint8_t p2pos)
    : player{{0, p1pos}, {0, p2pos}}
    { }
    
    GameState(const GameState& other)
    : player{other.player[0], other.player[1]}
    { }
};

static void quantumRound(uint8_t player, uint64_t count, const GameState& gs, uint64_t (&wins)[2])
{
    for (auto o = 0; o < sizeof_array(cx_outcomes); o++)
    {
        auto q = cx_outcomes[o][1] * count;
        auto gp(gs);

        gp.player[player].position += cx_outcomes[o][0];
        gp.player[player].position %= 10;
        gp.player[player].score += (gp.player[player].position + 1);

        if (gp.player[player].score >= 21)
            wins[player] += q;
        else
            quantumRound((player + 1) & 1, q, gp, wins);
    }
}

}

int main()
{
    using namespace aoc;
    
    GameState gs(uint8_t(cx_starts[0] - 1), uint8_t(cx_starts[1] - 1));
    uint64_t wins[2] = { 0ull, 0ull };

    quantumRound(0, 1, gs, wins);

    cout << wins[0] << '\n';
    cout << wins[1] << '\n';

    return 0;
}
