#include <algorithm>
#include <iostream>
#include <queue>

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

struct Player
{
    uint16_t player : 1;
    uint16_t position : 4;
    uint16_t score : 11;
};

}

int main()
{
    using namespace aoc;

    queue<Player> ps;
    for (uint16_t i = 0; i < sizeof_array(cx_starts); i++)
        ps.push(Player{i, uint16_t(cx_starts[i] - 1), 0});

    uint64_t count = 0;
    //uint8_t die = 0;
    uint64_t wins[2] = { 0ull, 0ull };

    while (!ps.empty())
    {
        auto p = ps.front();
        ps.pop();

        for (auto outcome = 0; outcome < sizeof(cx_outcomes); outcome++)
        {
            auto pq = p;
            // for (auto j = 0; j < 3; j++)
            // {
            //     count++;
            //     //die = (die % 100) + 1;
            //     die = cx_outcomes[outcome][j];
            //     pq.position += die;
            //     pq.position %= 10;
            // }
            count += 3;
            pq.position += cx_outcomes[outcome][0];
            pq.position %= 10;
            pq.score += (pq.position + 1);
            if (pq.score >= 21)
                wins[pq.player] += cx_outcomes[outcome][1];
            else
                ps.push(pq);
        }

        if (count % 300000 == 0)
            cout << count << '\n';
    }

    cout << wins[0] << '\n';
    cout << wins[1] << '\n';

    return 0;
}
