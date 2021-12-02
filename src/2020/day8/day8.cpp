#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

int main()
{
    using namespace std;
    using namespace std;
    
    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    enum class Op { nop, acc, jmp, };
    using Instruction = tuple<Op, int, bool>;
    vector<Instruction> program;

    string line;
    while (getline(inputFile, line, '\n'))
    {
        auto& [op, value, executed] = program.emplace_back();
        if (line.find("nop") == 0)
        {
            op = Op::nop;
            value = stoi(line.substr(3));
        }
        else if (line.find("acc") == 0)
        {
            op = Op::acc;
            value = stoi(line.substr(3));
        }
        else if (line.find("jmp") == 0)
        {
            op = Op::jmp;
            value = stoi(line.substr(3));
        }
    }

    unsigned pc = 0;
    int acc = 0;
    while (pc < program.size())
    {
        auto& [op, value, executed] = program[pc];

        if (executed)
            break;

        executed = true;

        switch (op)
        {
        case Op::nop:
            break;
        case Op::acc:
            acc += value;
            break;
        case Op::jmp:
            pc += value;
            continue;
        default:
            break;
        }

        ++pc;
    }

    cout << acc << "\n";

    return 0;
}

