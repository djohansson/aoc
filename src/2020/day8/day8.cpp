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
    using Instruction = tuple<Op, int>;
    using Program = vector<Instruction>;
    using State = tuple<unsigned, int, vector<bool>>; //pc, acc, executed

    Program program;
    string line;
    while (getline(inputFile, line, '\n'))
    {
        auto& [op, value] = program.emplace_back();

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

    auto execute = [](const Program& program, State& state)
    {
        auto& [pc, acc, executed] = state;

        executed.resize(program.size());
        
        while (pc < program.size())
        {
            const auto& [op, value] = program[pc];

            if (executed[pc])
                return false;

            executed[pc] = true;

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

        return true;
    };

    // part 1

    {
        State state;
        bool result = execute(program, state);
        auto& [pc, acc, executed] = state;
        
        cout << "part1 result: " << result << "\n";
        cout << "pc: " << pc << "\n";
        cout << "acc: " << acc << "\n";
    }

    // part 2

    State state;
    for (int i = program.size() - 1; i >= 0; --i)
    {
        Program program2 = program;

        auto& [op, value] = program2[i];

        switch (op)
        {
        case Op::nop:
            op = Op::jmp;
            break;
        case Op::jmp:
            op = Op::nop;
            break;
        default:
            break;
        }

        State state;
        bool result = execute(program2, state);
        auto& [pc, acc, executed] = state;

        if (result)
        {
            cout << "part2 result: " << result << "\n";
            cout << "pc: " << pc << "\n";
            cout << "acc: " << acc << "\n";

            return 0;
        }
    }

    return -1;
}

