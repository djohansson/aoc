#include <algorithm>
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

}

int main()
{
    using namespace aoc;

    unsigned scores[] = { 0, 0 };
    unsigned positions[] = { 0, 0 };
    for (auto i = 0; i < sizeof_array(positions); i++)
        positions[i] = cx_starts[i] - 1; // 0-based position

    unsigned count = 0;
    unsigned die = 0;

    while (true)
    {
        for (auto i = 0; i < sizeof_array(scores); i++)
        {
            for (auto j = 0; j < 3; j++)
            {
                count++;
                die = (die % 100) + 1;
                positions[i] += die;
                positions[i] %= 10;
            }

            scores[i] += (positions[i] + 1);
            cout << scores[i] << '\n';

            if (scores[i] >= 1000)
                goto end;
        }
    }

end:

    auto minScore = min_element(begin(scores), end(scores));

    cout << count * *minScore;

    return 0;
}
