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
        Type(const char* str) : id(str) {}

        bool operator<(const Type& b) const
        {
            return id < b.id;
        }

        unsigned contains()
        {
            unsigned result = 0;

            for (const auto& [type, count] : contents)
                result += (type->contains() + 1) * count;

            return result;
        }

        unsigned contains(const string& id)
        {
            unsigned result = 0;

            for (const auto& [type, count] : contents)
                result += (type->id == id ? count : type->contains(id));
            
            return result;
        }

        string id;
        std::vector<pair<shared_ptr<Type>, unsigned>> contents;
    };

    struct TypeCompare : std::less<void>
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

        using is_transparent = int;
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
        auto [typeIt, result] = types.emplace(make_shared<Type>(str.c_str()));

        auto prev = next + 8;
        do
        {
            next = line.find_first_of(",.", prev);
            if (!next || next == string::npos)
                break;

            str = line.substr(prev, next - prev);
            if (str.compare("no other bags") == 0)
                break;

            size_t skip = 0;
            int count = stoi(str, &skip);
            auto [type2It, result] = types.emplace(make_shared<Type>(str.substr(skip + 1, str.find("bag") - (skip + 1) - 1).c_str()));

            (*typeIt)->contents.emplace_back(make_pair(*type2It, count));

            prev = next + 2;

        } while (next != string::npos);
    }

    unsigned result{};

    // part 1
    for (const auto& type : types)
        result += (type->contains("shiny gold") > 0);

    cout << result << "\n";

    // part 2
    result = (*types.find("shiny gold"))->contains();

    cout << result << "\n";

    return 0;
}
