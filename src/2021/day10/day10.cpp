
#include <algorithm>
#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include <vector>

namespace aoc
{

using namespace std;

enum class Op { open, close, invalid };

Op op(char c)
{
    switch (c)
    {
    case '{':
    case '[':
    case '(':
    case '<':
        return Op::open;
    case '}':
    case ']':
    case ')':
    case '>':
        return Op::close;
    default:
        break;
    }

    return Op::invalid;
}

char closes(char c)
{
    switch (c)
    {
    case '{':
        return '}';
    case '[':
        return ']';
    case '(':
        return ')';
    case '<':
        return '>';
    default:
        break;
    }

    return 0;
}

}

int main()
{
    using namespace aoc;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    unsigned total = 0;
    vector<uint64_t> scores;
    
    string line;
    while (getline(inputFile, line, '\n'))
    {
        unsigned highscore = 0;
        stack<char> s;

        for (auto c : line)
        {
            switch (op(c))
            {
            case Op::open:
                s.push(c);
                break;
            case Op::close:
                if (c != closes(s.top()))
                    highscore += (c == ')' ? 3 : c == ']' ? 57 : c == '}' ? 1197 : c == '>' ?  25137 : 0);
                s.pop();
                break;
            case Op::invalid:
            default:
                continue;
            }
        }

        total += highscore;
        
        if (!highscore)
        {
            auto& score = scores.emplace_back();
            while (!s.empty())
            {
                score *= 5;
                switch (closes(s.top()))
                {
                case ')':
                    score += 1;
                    break;
                case ']':
                    score += 2;
                    break;
                case '}':
                    score += 3;
                    break;
                case '>':
                    score += 4;
                    break;
                default:
                    break;
                }
                s.pop();
            }
        }
    }

    // part 1
    cout << "part 1: " << total << "\n";
    
    // part 2
    sort(begin(scores), end(scores));
    cout << "part 2: " << scores[scores.size() / 2] << "\n";

    return 0;
}
