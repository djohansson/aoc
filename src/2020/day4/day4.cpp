#include <cassert>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <string>
#include <string_view>
#include <utility>

int main()
{
    using namespace std;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    string inputString;
    inputFile.seekg(0, ios::end);   
    inputString.reserve(inputFile.tellg());
    inputFile.seekg(0, ios::beg);
    inputString.assign((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();

    static const regex sc_doubleNewlineRegex("\\n\\n");
    list<string> passports(
        sregex_token_iterator(inputString.begin(), inputString.end(), sc_doubleNewlineRegex, -1),
        sregex_token_iterator());

    uint32_t validCount1 = 0, validCount2 = 0;
    auto passportIt = passports.begin();
    while (passportIt != passports.end())
    {
        const auto& passport = *passportIt;

        static const regex sc_tokenRegex("[\\n\\r\\s]+");
        list<string> tokens(
            sregex_token_iterator(passport.begin(), passport.end(), sc_tokenRegex, -1),
            sregex_token_iterator());

        assert(tokens.size() <= 8);

        uint8_t keyFlags = 0;
        uint8_t valueFlags = 0;

        auto tokenIt = tokens.begin();
        while (tokenIt != tokens.end())
        {
            static const map<string_view, pair<regex, uint8_t>> sc_keyValueRegexMap =
            {
                {"byr",{regex("19[2-9][0-9]|200[0-2]"), 0b00000001}},
                {"iyr",{regex("201[0-9]|2020"), 0b00000010}},
                {"eyr",{regex("202[0-9]|2030"), 0b00000100}},
                {"hgt",{regex("((1[5678][0-9])|(19[0-3]))cm|(59|6[0-9]|7[0-6])in"), 0b00001000}},
                {"hcl",{regex("#([a-f0-9]){6}"), 0b00010000}},
                {"ecl",{regex("amb|blu|brn|gry|grn|hzl|oth"), 0b00100000}},
                {"pid",{regex("[0-9]{9}"), 0b01000000}},
            };

            const auto& token = *tokenIt;
            const auto keyValIt = sc_keyValueRegexMap.find(token.substr(0, 3));
            if (keyValIt != sc_keyValueRegexMap.end())
            {
                const auto& [key, valueRegexPair] = *keyValIt;
                const auto& [valueRegex, mask] = valueRegexPair;

                keyFlags |= mask;

                auto valueString = token.substr(4);
                smatch valueMatch;
                if (regex_match(valueString, valueMatch, valueRegex))
                {
                    valueFlags |= mask;
                }
            }

            tokenIt++;
        }

        if (keyFlags == 0b01111111)
            validCount1++;
        
        if (valueFlags == 0b01111111)
            validCount2++;

        passportIt++;
    }

    cout << "validCount1: " << validCount1 << "\n";
    cout << "validCount2: " << validCount2 << "\n";

    return 0;
}
