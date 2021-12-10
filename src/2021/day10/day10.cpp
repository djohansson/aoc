
#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <vector>

namespace aoc
{

using namespace std;

struct Chunk
{
    char c[2] = { '\0', '\0' };
    weak_ptr<Chunk> parent;
    list<shared_ptr<Chunk>> children;
};

enum class CharType { open, close, invalid, empty };

CharType getType(char c)
{
    switch (c)
    {
    case '{':
    case '[':
    case '(':
    case '<':
        return CharType::open;
    case '}':
    case ']':
    case ')':
    case '>':
        return CharType::close;
    case '\0':
        return CharType::empty;
    default:
        break;
    }

    return CharType::invalid;
}

char matches(char c)
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

    return '\0';
}

bool matches(const Chunk& chunk)
{
    switch (chunk.c[0])
    {
    case '{':
        return chunk.c[1] == '}';
    case '[':
        return chunk.c[1] == ']';
    case '(':
        return chunk.c[1] == ')';
    case '<':
        return chunk.c[1] == '>';
    case '\0':
        return chunk.c[1] == '\0';
    default:
        break;
    }

    return false;
}

bool open(const Chunk& chunk)
{
    return getType(chunk.c[0]) == CharType::open && getType(chunk.c[1]) == CharType::empty;
}

bool closed(const Chunk& chunk)
{
    return getType(chunk.c[0]) == CharType::open && getType(chunk.c[1]) == CharType::close;
}

bool invalid(char c)
{
    return getType(c) == CharType::invalid;
}

bool invalid(const Chunk& chunk)
{
    return invalid(chunk.c[0]) || invalid(chunk.c[1]);
}

bool empty(const Chunk& chunk)
{
    return getType(chunk.c[0]) == CharType::empty && getType(chunk.c[1]) == CharType::empty;
}

bool isComplete(const Chunk& chunk)
{
    return matches(chunk) && all_of(begin(chunk.children), end(chunk.children), [](const auto& child)
    {
        return isComplete(*child);
    });
}

void complete(const Chunk& chunk, string& str)
{
    for (const auto& child : chunk.children)
        complete(*child, str);

    if (open(chunk))
        str.push_back(matches(chunk.c[0]));
}

}

int main()
{
    using namespace aoc;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    unsigned total = 0;
    unsigned result = 0;

    string line;
    vector<shared_ptr<Chunk>> incomplete;
    while (getline(inputFile, line, '\n'))
    {
        unsigned firstValue = 0;
        unsigned highscore = 0;

        auto root = make_shared<Chunk>();
        auto current = *root->children.emplace(root->children.end(), make_shared<Chunk>());
        current->parent = root;
        
        for (auto c : line)
        {
            if (!current)
                break;

            switch (getType(c))
            {
            case CharType::open:
                {
                    if (open(*current))
                    {
                        auto childIt = current->children.emplace(current->children.end(), make_shared<Chunk>());
                        (*childIt)->parent = current;
                        current = (*childIt);
                    }

                    current->c[0] = c;
                    cout << c;
                }
                break;
            case CharType::close:
                if (open(*current))
                {
                    current->c[1] = c;

                    if (matches(*current))
                        cout << c;
                    else
                    {
                        auto value = (c == ')' ? 3 : c == ']' ? 57 : c == '}' ? 1197 : c == '>' ?  25137 : 0);
                        firstValue = firstValue ? firstValue : value;
                        highscore += value;
                        cout << "!" << current->c[1] << "!";
                    }

                    current = current->parent.lock();
                }
                else
                {
                    cout << "?" << c << "?";
                    current = nullptr;
                }
                break;
            case CharType::invalid:
            default:
                current = nullptr;
                continue;
            }
        }

        total += highscore;
        result += firstValue;

        if (isComplete(*root))
        {
            cout << " ; complete";
        }
        else if (highscore)
        {
            cout << " ; broken, highscore: " << highscore;
        }
        else
        {
            incomplete.emplace_back(move(root));
            cout << "; incomplete";
        }

        cout << "\n";
    }

    // part 1
    cout << "total: " << total << "\n";
    cout << "result: " << result << "\n";

    // part 2
    vector<uint64_t> scores;
    for (auto root : incomplete)
    {
        auto& score = scores.emplace_back();

        string wouldComplete;
        complete(*root, wouldComplete);
        cout << wouldComplete << "\n";

        for (char c : wouldComplete)
        {
            score *= 5;

            switch (c)
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
        }

        cout << "score: " << score << "\n";
    }

    sort(begin(scores), end(scores));

    cout << "result: " << scores[scores.size() / 2] << "\n";

    return 0;
}
