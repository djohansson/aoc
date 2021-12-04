#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <tuple>
#include <vector>

std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        if (item.length() > 0)
            elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string& s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

int main()
{
    using namespace std;

    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
        return -1;

    vector<unsigned> numbers;
    {
        string firstLine;
        if (!getline(inputFile, firstLine, '\n'))
            return -1;

        vector<string> numberStrings = split(firstLine, ',');
        for (const auto& numberString : numberStrings)
            numbers.push_back(stoi(numberString));

        string secondLine;
        if (!getline(inputFile, secondLine, '\n'))
            return -1;

        assert(secondLine.size() == 0);
    }

    using BingoBoard = tuple<map<unsigned, tuple<unsigned, unsigned>>, vector<unsigned>, vector<unsigned>, bool>;

    size_t rowCount = 0;
    size_t colCount = 0;

    vector<BingoBoard> boards;
    {
        BingoBoard board;
        string bingoLine;
        unsigned row = 0;
        while (getline(inputFile, bingoLine, '\n'))
        {
            vector<string> lineNumbers = split(bingoLine, ' ');
            colCount = max(lineNumbers.size(), colCount);
            unsigned col = 0;
            for (const auto& lineNumber : lineNumbers)
            {
                auto& [boardMap, rowMasks, colMasks, bingo] = board;
                boardMap[stoi(lineNumber)] = make_tuple(row, col);

                ++col;
            }

            ++row;

            if (inputFile.peek() == '\n')
            {
                rowCount = max(size_t(row), rowCount);
                boards.emplace_back(move(board));
                row = 0;
                inputFile.get();
            };
        }
        boards.emplace_back(move(board));
    }

    for (auto& [boardMap, rowMasks, colMasks, bingo] : boards)
    {
        rowMasks.resize(rowCount);
        colMasks.resize(colCount);
    }

    for (auto number : numbers)
    {
        for (auto& board : boards)
        {
            auto& [boardMap, rowMasks, colMasks, bingo] = board;
            auto boardIt = boardMap.find(number);
            if (bingo || boardIt == boardMap.end())
                continue;

            const auto& [row, col] = boardIt->second;
            rowMasks[row] |= 1 << col;
            colMasks[col] |= 1 << row;

            auto checkBingo = [&board, &boardIt, &colCount, &rowCount, &number]()
            {
                auto& [boardMap, rowMasks, colMasks, bingo] = board;
                const auto& [row, col] = boardIt->second;
                if (rowMasks[row] == ((1 << colCount) - 1) || colMasks[col] == ((1 << rowCount) - 1))
                {
                    unsigned unmarkedSum = 0;
                    for (const auto& [bNumber, bRowCol] : boardMap)
                    {
                        const auto& [bRow, bCol] = bRowCol;

                        bool rowMarked = rowMasks[bRow] & (1 << bCol);
                        bool colMarked = colMasks[bCol] & (1 << bRow);

                        assert(rowMarked == colMarked);

                        if (!rowMarked && !colMarked)
                            unmarkedSum += bNumber;
                    }

                    bingo = true;

                    return unmarkedSum * number;
                }

                return 0u;
            };

            if (unsigned result = checkBingo(); result)
                cout << result << "\n";
        }
    }

    return 0;
}
