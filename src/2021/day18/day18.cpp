#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <variant>
#include <vector>

namespace aoc
{

using namespace std;

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

struct Node;
using Single = std::variant<unsigned, shared_ptr<Node>>;
using Double = array<Single, 2>;
struct Node : public Double { weak_ptr<Node> parent; };

static inline void printDouble(const Double& d);

static inline void printSingle(const Single& s)
{
    if (s.index() == variant_npos)
    {
        cout << "invalid";
        return;
    }

    visit(overloaded
    {
        [](auto arg) { cout << arg; },
        [](const shared_ptr<Node>& arg) { printDouble(*arg); }
    }, s);
}

static inline void printDouble(const Double& d)
{
    cout << '[';
    printSingle(d[0]);
    cout << ',';
    printSingle(d[1]);
    cout << ']';
}

static inline void explodeAddUp(Node& node, unsigned value, bool isLeft)
{
    if (value == 0)
        return;

    if (holds_alternative<unsigned>(node[isLeft ? 1 : 0]))
        get<unsigned>(node[isLeft ? 1 : 0]) += value;

    if (holds_alternative<shared_ptr<Node>>(node[isLeft ? 1 : 0]))
        explodeAddUp(*get<shared_ptr<Node>>(node[isLeft ? 1 : 0]), value, isLeft);
}

static inline void explodeAddDown(Node& node, unsigned value, bool isLeft)
{
    auto parent = node.parent.lock();
    if (!parent || value == 0)
        return;

    if (holds_alternative<shared_ptr<Node>>((*parent)[isLeft ? 1 : 0]))
    {
        if (&node == get<shared_ptr<Node>>((*parent)[isLeft ? 1 : 0]).get())
        {
            if (holds_alternative<unsigned>((*parent)[isLeft ? 0 : 1]))
                get<unsigned>((*parent)[isLeft ? 0 : 1]) += value;
            else
                explodeAddUp(*get<shared_ptr<Node>>((*parent)[isLeft ? 0 : 1]), value, isLeft);

            return;
        }
    }

    explodeAddDown(*parent, value, isLeft);
}

static inline unsigned explode(Single& s, unsigned depth);

static inline bool explode(Node& d, unsigned depth)
{
    unsigned val = explode(d[0], depth);
    if (holds_alternative<unsigned>(d[0]))
    {
        explodeAddDown(d, val, true);
        val = explode(d[1], depth);
        if (holds_alternative<unsigned>(d[1]))
        {
            explodeAddDown(d, val, false);
            return val > 0;
        }
    }

    return false;
}

static inline unsigned explode(Single& s, unsigned depth)
{
    assert(s.index() != variant_npos);

    unsigned val = 0;
    
    visit(overloaded
    {
        [&s, &val, depth](auto arg)
        {
            if (depth >= 4)
            {
                s = 0u;
                val = arg;
            }
        },
        [&s, depth](const shared_ptr<Node>& arg)
        {
            if (explode((*arg), depth + 1))
                s = 0u;
        }
    }, s);

    return val;
}

}

int main()
{
    using namespace aoc;

    ifstream inputFile("test7.txt");
    if (!inputFile.is_open())
        return -1;

    shared_ptr<Node> root;
    stack<bool> isSecond;
    
    string line;
    while (getline(inputFile, line, '\n'))
    {
        cout << line << '\n';

        auto current = root;
        isSecond.push(false);
        
        for (auto c : line)
        {
            switch (c)
            {
            case '[':
            {
                auto node = make_shared<Node>();
                if (current/* != root*/)
                {
                    node->parent = current;
                    (*current)[isSecond.top()] = node;
                }
                else
                {
                    if (root)
                    {
                        auto oldRoot = root;
                        root = make_shared<Node>();
                        oldRoot->parent = root;
                        node->parent = root;
                        (*root)[0] = oldRoot;
                        (*root)[1] = node;
                    }
                    else
                    {
                        root = node;
                    }
                }
                current = node;
                isSecond.push(false);
                break;
            }
            case ',':
            {
                assert(!isSecond.top());
                isSecond.top() = true;
                break;
            }
            case ']':
            {
                current = current->parent.lock();
                assert(isSecond.top());
                isSecond.pop();
                break;
            }
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                (*current)[isSecond.top()] = static_cast<unsigned>(c - '0');
                break;
            }
            default:
                break;
            }
        }

        printDouble(*root);

        cout << '\n';

        explode(*root, 0);

        printDouble(*root);

        cout << '\n';
    }
    
    return 0;
}
