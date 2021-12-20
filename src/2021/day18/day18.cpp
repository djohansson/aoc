#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

namespace aoc
{

using namespace std;

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

struct Node;
using Number = optional<unsigned>;
using Single = std::variant<Number, shared_ptr<Node>>;
using Double = array<Single, 2>;
struct Node : public Double { weak_ptr<Node> parent; };

static inline void print(const shared_ptr<Node>& d);

static inline void print(const Single& s)
{
    if (s.index() == variant_npos)
    {
        cout << "invalid";
        return;
    }

    visit(overloaded
    {
        [](auto v) { cout << v.value_or(-1); },
        [](const shared_ptr<Node>& n) { print(n); }
    }, s);
}

static inline void print(const shared_ptr<Node>& d)
{
    cout << '[';
    print((*d)[0]);
    cout << ',';
    print((*d)[1]);
    cout << ']';
}

static inline shared_ptr<Node> add(const shared_ptr<Node>& a, const shared_ptr<Node>& b)
{
    auto node = make_shared<Node>();
    a->parent = node;
    b->parent = node;
    (*node)[0] = a;
    (*node)[1] = b;

    return node;
}

static inline void explodeAddUp(Node& node, unsigned value, bool isLeft)
{
    if (value == 0)
        return;

    if (holds_alternative<Number>(node[isLeft ? 1 : 0]))
        get<Number>(node[isLeft ? 1 : 0]).value() += value;

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
            if (holds_alternative<Number>((*parent)[isLeft ? 0 : 1]))
                get<Number>((*parent)[isLeft ? 0 : 1]).value() += value;
            else
                explodeAddUp(*get<shared_ptr<Node>>((*parent)[isLeft ? 0 : 1]), value, isLeft);

            return;
        }
    }

    explodeAddDown(*parent, value, isLeft);
}

static inline tuple<Number, bool> explode(Single& s, unsigned depth);

static inline tuple<Number, Number, bool> explode(const shared_ptr<Node>& d, unsigned depth)
{
    auto [val0, hasExploded0] = explode((*d)[0], depth);

    if (hasExploded0 && val0 && holds_alternative<Number>((*d)[0]))
    {
        auto [val1, hasExploded1] = explode((*d)[1], depth);
        
        if (hasExploded1 && val1 && holds_alternative<Number>((*d)[1]))
        {
            assert(hasExploded0 && hasExploded1);
            
            explodeAddDown(*d, val0.value(), true);
            explodeAddDown(*d, val1.value(), false);

            return {val0, val1, true};
        }
        
        return {{}, {}, hasExploded0 || hasExploded1};
    }

    if (!hasExploded0)
    {
        auto [val1, hasExploded1] = explode((*d)[1], depth);
        return {{}, {}, hasExploded1};
    }

    return {{}, {}, hasExploded0};
}

static inline tuple<Number, bool> explode(Single& s, unsigned depth)
{
    assert(s.index() != variant_npos);

    tuple<Number, bool> result;
    
    visit(overloaded
    {
        [&result, depth](auto v)
        {
            auto& [val, hasExploded] = result;
            if (depth >= 4)
            {
                val = v;
                hasExploded = true;
            }
            else
            {
                val.reset();
                hasExploded = false;
            }
        },
        [&s, &result, depth](const shared_ptr<Node>& n)
        {
            auto [val0, val1, boom] = explode(n, depth + 1);
            auto& [val, hasExploded] = result;
            val.reset();
            hasExploded = boom;
            if (hasExploded && val0 && val1)
                s = 0u;
        }
    }, s);

    return result;
}

static inline tuple<shared_ptr<Node>, bool> split(Single& s);

static inline tuple<shared_ptr<Node>, bool> split(const shared_ptr<Node>& d)
{
    auto [val0, hasSplit0] = split((*d)[0]);
    
    if (!hasSplit0)
        return split((*d)[1]);

    return {val0, hasSplit0};
}

static inline tuple<shared_ptr<Node>, bool> split(Single& s)
{
    assert(s.index() != variant_npos);

    tuple<shared_ptr<Node>, bool> result;
    
    visit(overloaded
    {
        [&s, &result](auto v)
        {
            auto& [node, hasSplit] = result;
            if (v > 9)
            {
                hasSplit = true;
                node = make_shared<Node>();
                (*node)[0] = Number(v.value() / 2);
                (*node)[1] = Number((v.value() + 1) / 2);
                s = node;
            }
        },
        [&result](const shared_ptr<Node>& n)
        {
            result = split(n);
            auto& [node, hasSplit] = result;
            if (hasSplit && node)
            {
                node->parent = n;
                node = nullptr;
            }
        }
    }, s);

    return result;
}

static inline unsigned magnitude(Single& s);

static inline unsigned magnitude(const shared_ptr<Node>& d)
{
    return 3 * magnitude((*d)[0]) + 2 * magnitude((*d)[1]);
}

static inline unsigned magnitude(Single& s)
{
    assert(s.index() != variant_npos);

    unsigned result = 0;
    
    visit(overloaded
    {
        [&result](auto v)
        {
            result = v.value();
        },
        [&result](const shared_ptr<Node>& n)
        {
            result = magnitude(n);
        }
    }, s);

    return result;
}

}

int main()
{
    using namespace aoc;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    shared_ptr<Node> root;
    stack<bool> isSecond;
    bool wasAdded = false;
    
    string line;
    while (getline(inputFile, line, '\n'))
    {
        shared_ptr<Node> current;
        
        for (auto c : line)
        {
            switch (c)
            {
            case '[':
            {
                //cout << c;
                auto node = make_shared<Node>();
                if (current)
                {
                    node->parent = current;
                    (*current)[isSecond.top()] = node;
                }
                else
                {
                    root = root ? add(root, node) : node;
                }
                current = node;
                isSecond.push(false);
                break;
            }
            case ',':
            {
                //cout << c;
                assert(!isSecond.top());
                isSecond.top() = true;
                break;
            }
            case ']':
            {
                //cout << c;
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
                //cout << c;
                (*current)[isSecond.top()] = Number(static_cast<unsigned>(c - '0'));
                break;
            }
            default:
                assert(false);
            }
        }

        //cout << '\n';

        if (wasAdded)
            cout << "+ ";
        else
            cout << "  ";

        print(root);
        cout << '\n';

        wasAdded = true;

        tuple<Number, Number, bool> explodeResult;
        auto& [val0, val1, wasExploded] = explodeResult;

        tuple<shared_ptr<Node>, bool> splitResult;
        auto& [node, wasSplit] = splitResult;

        do
        {
            do
            {
                explodeResult = explode(root, 0);

                // if (wasExploded)
                // {
                //     cout << "E ";
                //     print(root);
                //     cout << '\n';
                // }
            } while (wasExploded);

            splitResult = split(root);

            // if (wasSplit)
            // {
            //     cout << "S ";
            //     print(root);
            //     cout << '\n';
            // }

        } while (wasSplit);

        if (wasAdded)
        {
            cout << "= ";
            print(root);
            cout << '\n';
        }

        cout << "M " << magnitude(root);
        cout << '\n';
    }
    
    return 0;
}
