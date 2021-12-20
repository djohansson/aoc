#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <numeric>
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
    if (!d) return;

    cout << '[';
    print((*d)[0]);
    cout << ',';
    print((*d)[1]);
    cout << ']';
}

static inline shared_ptr<Node> add(const shared_ptr<Node>& a, const shared_ptr<Node>& b)
{
    if (!a) return b;
    if (!b) return a;

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

    if (hasExploded0 && val0) // only true if (*d)[0] is a literal
    {
        auto [val1, hasExploded1] = explode((*d)[1], depth);
        
        if (hasExploded1 && val1) // only true if (*d)[1] is a literal
        {
            assert(hasExploded0 && hasExploded1);
            
            explodeAddDown(*d, val0.value(), true);
            explodeAddDown(*d, val1.value(), false);

            return {val0, val1, true};
        }
        
        return {val0, val1, hasExploded0 || hasExploded1};
    }

    if (hasExploded0) // early out if already exploded
        return {val0, {}, hasExploded0};

    return tuple_cat(make_tuple(0u), explode((*d)[1], depth));
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
            val = v;
            hasExploded = (depth >= 4);
        },
        [&s, &result, depth](const shared_ptr<Node>& n)
        {
            auto [val0, val1, boom] = explode(n, depth + 1);
            auto& [_not_used_, hasExploded] = result;
            hasExploded = boom;
            if (hasExploded && val0 && val1) // if n is an exploded pair with two literals
                s = 0u; // replace with 0 literal
        }
    }, s);

    return result;
}

static inline tuple<shared_ptr<Node>, bool> split(Single& s);

static inline tuple<shared_ptr<Node>, bool> split(const shared_ptr<Node>& d)
{
    auto [val0, hasSplit0] = split((*d)[0]);
    
    if (hasSplit0)
        return {val0, hasSplit0};
        
    return split((*d)[1]);
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

static inline shared_ptr<Node> reduce(const shared_ptr<Node>& n)
{
    tuple<Number, Number, bool> explodeResult;
    auto& [val0, val1, wasExploded] = explodeResult;

    tuple<shared_ptr<Node>, bool> splitResult;
    auto& [node, wasSplit] = splitResult;

    do
    {
        do { explodeResult = explode(n, 0); } while (wasExploded);
        splitResult = split(n);
    } while (wasSplit);

    return n;
}

static inline Single copy(const Single& s);

static inline shared_ptr<Node> copy(const shared_ptr<Node>& n)
{
    shared_ptr<Node> result = make_shared<Node>();
    
    (*result)[0] = copy((*n)[0]);
    if (holds_alternative<shared_ptr<Node>>((*result)[0]))
        get<shared_ptr<Node>>((*result)[0])->parent = result;

    (*result)[1] = copy((*n)[1]);
    if (holds_alternative<shared_ptr<Node>>((*result)[1]))
        get<shared_ptr<Node>>((*result)[1])->parent = result;
    
    return result;
}

static inline Single copy(const Single& s)
{
    assert(s.index() != variant_npos);

    Single result;
    
    visit(overloaded
    {
        [&result](auto v)
        {
            result = v;
        },
        [&result](const shared_ptr<Node>& n)
        {
            result = copy(n);
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

    vector<shared_ptr<Node>> numbers;
    stack<bool> isSecond;
    shared_ptr<Node> root;
    
    string line;
    while (getline(inputFile, line, '\n'))
    {
        shared_ptr<Node> row;
        shared_ptr<Node> current;
        
        for (auto c : line)
        {
            switch (c)
            {
            case '[':
            {
                auto node = make_shared<Node>();
                if (current)
                {
                    node->parent = current;
                    (*current)[isSecond.top()] = node;
                }
                else
                {
                    row = node;
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
                (*current)[isSecond.top()] = Number(static_cast<unsigned>(c - '0'));
                break;
            }
            default:
                assert(false);
            }
        }

        numbers.emplace_back(row);
    }

    root = accumulate(begin(numbers), end(numbers), shared_ptr<Node>{}, [](auto prev, auto n)
    {
        return reduce(copy(add(prev, n)));
    });

    cout << "M " << magnitude(root);
    cout << '\n';

    unsigned maxSum = 0u;

    for (const auto& a : numbers)
    {
        for (unsigned i = 0; i < numbers.size(); i++)
        {
            const auto& b = *(begin(numbers) + i);

            if (a.get() == b.get())
                continue;
            
            auto c = add(a, b);
            auto cr = reduce(copy(c));
            auto cm = magnitude(cr);
            auto d = add(b, a);
            auto dr = reduce(copy(d));
            auto dm = magnitude(dr);

            maxSum = max(maxSum, max(cm, dm));
        }
    }

    cout << "MS " << maxSum;
    cout << '\n';
    
    return 0;
}
