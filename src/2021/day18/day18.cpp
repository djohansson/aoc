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

struct Pair;
using Single = std::variant<unsigned, shared_ptr<Pair>>;
using Double = array<Single, 2>;
struct Pair : public Double
{
    //Pair() : Double() {}
    //Pair(Double&& d, const shared_ptr<Pair>& p) : Double(exchange(d, {})), parent(p) {}
    weak_ptr<Pair> parent;
};

static inline void printDouble(const Double& d);

static inline void printSingle(const Single& s)
{
    if (s.index() == variant_npos)
    {
        cout << "invalid";
        return;
    }

    visit(overloaded {
        [](auto arg) { cout << arg; },
        [](const shared_ptr<Pair>& arg) { printDouble(static_cast<const Double&>(*arg)); }
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

}

int main()
{
    using namespace aoc;

    ifstream inputFile("test.txt");
    if (!inputFile.is_open())
        return -1;

    shared_ptr<Pair> root;
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
                auto node = make_shared<Pair>();
                if (current != root)
                {
                    node->parent = current;
                    static_cast<Double&>(*current)[isSecond.top()] = node;
                    isSecond.push(false);
                    
                }
                else
                {
                    if (root)
                    {
                        auto oldRoot = root;
                        root = make_shared<Pair>();
                        oldRoot->parent = root;
                        node->parent = root;
                        static_cast<Double&>(*root)[0] = oldRoot;
                        static_cast<Double&>(*root)[1] = node;
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
                static_cast<Double&>(*current)[isSecond.top()] = static_cast<unsigned>(c - '0');
                break;
            }
            default:
                break;
            }
        }

        printDouble(static_cast<Double&>(*root));

        cout << '\n';
    }
    
    return 0;
}
