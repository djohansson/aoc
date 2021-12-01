#pragma once

#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <utility>

int main()
{
    using namespace std;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    struct Type
    {
        Type(string&& s) : id(move(s)) {}

        bool operator<(const Type& b) const
        {
            return id < b.id;
        }

        unsigned count(const string& id)
        {
            unsigned result{};

            for (const auto& [type, count] : contents)
                result += (type->id == id ? count : type->count(id));
            
            return result;
        }

        string id;
        std::vector<pair<shared_ptr<Type>, unsigned>> contents;
    };

    struct TypeCompare 
    {
        bool operator()(const shared_ptr<Type>& a, const shared_ptr<Type>& b) const
        {
            return *a < *b;
        }

        bool operator()(const shared_ptr<Type>& a, const Type& b) const
        {
            return *a < b;
        }

        bool operator()(const Type& a, const shared_ptr<Type>& b) const
        {
            return a < *b;
        }
    };

    set<shared_ptr<Type>, TypeCompare> types;

    string line;
    while (getline(inputFile, line, '\n'))
    {
        auto next = line.find("contain");
        if (!next || next == string::npos)
            continue;

        auto str = line.substr(0, next);
        str = str.substr(0, str.find("bag") - 1);
        auto [typeIt, result] = types.emplace(make_shared<Type>(move(str)));

        auto prev = next + 8;
        do
        {
            next = line.find_first_of(",.", prev);
            if (!next || next == string::npos)
                break;

            auto str = line.substr(prev, next - prev);
            if (str.compare("no other bags") == 0)
                break;

            size_t skip = 0;
            int count = stoi(str, &skip);
            auto [type2It, result] = types.emplace(make_shared<Type>(str.substr(skip + 1, str.find("bag") - (skip + 1) - 1)));

            (*typeIt)->contents.emplace_back(make_pair(*type2It, count));

            prev = next + 2;

        } while (next != string::npos);
    }

    unsigned result{};

    for (const auto& type : types)
        result += (type->count("shiny gold") > 0);

    cout << result << "\n";

    return 0;
}
