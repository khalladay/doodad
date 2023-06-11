#pragma once
#include <string>
#include <vector>

class DoodadModule
{
public:
	virtual bool CanHandleInput(std::string input) const = 0;
	virtual std::string HandleInput(std::string input) const = 0;

protected:
    std::vector<std::string> TokenizeString(const std::string& source, const std::string& delimiters) const
    {
        uint32_t iter = 0;

        std::vector<char> delims;
        for (uint32_t i = 0; i < delimiters.length(); ++i)
        {
            delims.push_back(delimiters[i]);
        }

        std::vector<char> whitespace;
        whitespace.push_back(' ');
        whitespace.push_back('\n');
        whitespace.push_back('\r\n');
        whitespace.push_back('\t');

        auto IsDelimeter = [](char c, std::vector<char> delimVec) -> bool
        {
            return std::find(delimVec.begin(), delimVec.end(), c) != delimVec.end();
        };

        auto IsWhitespace = [whitespace](char c) -> bool
        {
            return std::find(whitespace.begin(), whitespace.end(), c) != whitespace.end();
        };

        std::vector<std::string> tokens;
        std::string word;

        while (iter < source.length())
        {
            char c = source[iter];
            if (IsDelimeter(c, delims))
            {
                if (word.length() > 0) tokens.push_back(word);
                word = c;
                tokens.push_back(word);
                word = "";
            }
            else if (!IsWhitespace(c))
            {
                word.push_back(c);
            }
            iter++;
        }

        if (word.length() > 0)
        {
            tokens.push_back(word);
        }
        return tokens;
    }

};