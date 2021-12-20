#include <iostream>
#include <optional>
#include <limits>
#include <set>
#include <utility>

namespace aoc
{

using namespace std;

constexpr int cx_xmin = 34;
constexpr int cx_xmax = 67;
constexpr int cx_ymin = -215;
constexpr int cx_ymax = -186;

struct State
{
    int vx;
    int vy;
    int px;
    int py;
};

constexpr State cx_initial = {0, 0, 0, 0};

static inline void step(State& s)
{
    s.px += s.vx;
    s.py += s.vy;
    s.vx = (s.vx > 0 ? s.vx - 1 : (s.vx < 0 ? s.vx + 1 : 0));
    s.vy -= 1;
}

static inline void print(const State& s)
{
    cout << "State: [px: " << s.px << ", py: " << s.py << ", vx: " << s.vx << ", vy: "  <<  s.vy << "]\n";
}

}

int main()
{
    using namespace aoc;

    auto findMaxY = []()
    {
        set<pair<int, int>> velocities;
        int result = numeric_limits<int>::min();
        State initial = cx_initial;
        
        for (int vy = (cx_ymin - 1); vy < 1000; ++vy)
        {
            initial.vy = vy;

            for (int vx = 1; vx < (cx_xmax + 1); ++vx)
            {
                initial.vx = vx;

                State s = initial;
                int maxy = numeric_limits<int>::min();

                while (true)
                {
                    step(s);

                    maxy = max(maxy, s.py);

                    if (s.px < 0 || s.px > cx_xmax || s.py < cx_ymin)
                        break;

                    if (s.px <= cx_xmax && s.px >= cx_xmin && s.py <= cx_ymax && s.py >= cx_ymin)
                    {
                        velocities.emplace(make_pair(initial.vx, initial.vy));
                        result = max(result, maxy);

                        cout << "Found valid trajectory, max y: " << maxy << ", ";
                        print(s);
                    }
                }
            }
        }

        return make_pair(result, move(velocities));
    };

    auto [result, velocities] = findMaxY();

    cout << "\nResult: " << result;
    cout << "\nCount: " << velocities.size();
    
    return 0;
}
