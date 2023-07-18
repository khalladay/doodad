#include "doodad_module.h"
#include <regex>

bool DoodadModule::IsTokenSetValid(std::vector<std::string>& tokens) const
{
    bool lastWasNumeric = false;
    for (const auto& t : tokens)
    {
        if (lastWasNumeric)
        {
            if (IsNumeric(t)) return false;
        }
        lastWasNumeric = !lastWasNumeric;
    }

    return true;
}

bool DoodadModule::IsHex(std::string input) const
{
    std::regex hexRegex("0X([A-F0-9])+");
    std::smatch baseMatch;
    std::transform(input.begin(), input.end(), input.begin(), ::toupper);

    return std::regex_match(input, baseMatch, hexRegex);
}

bool DoodadModule::IsDecimal(std::string input) const
{
    std::regex decRegex("([0-9.])+");
    std::smatch baseMatch;
    return std::regex_match(input, baseMatch, decRegex);
}

bool DoodadModule::IsInteger(std::string input) const
{
    std::regex decRegex("([0-9])+");
    std::smatch baseMatch;
    return std::regex_match(input, baseMatch, decRegex);
}

bool DoodadModule::IsBinary(std::string input) const
{
    std::regex binRegex("0b([0-1])+");
    std::smatch baseMatch;
    return std::regex_match(input, baseMatch, binRegex);
}

bool DoodadModule::IsNumeric(std::string input) const
{
    return IsHex(input) || IsDecimal(input) || IsBinary(input);
}

float DoodadModule::DecimalToFloat(std::string input) const
{
    if (IsDecimal(input))
    {
        return atof(input.c_str());
    }
    return INFINITY;
}

inttype DoodadModule::NumericToInt(std::string input) const
{
    ERadix radix = GetRadix(input);

    bool outputPositive = true;
    if (radix == ERadix::Decimal)
    {
        if (input[0] == '-') outputPositive = false;
    }

    switch (radix)
    {
    case ERadix::Decimal:
    {
        if (outputPositive)
        {
            return _strtoui64(input.c_str(), nullptr, 10);
        }
        else
        {
            return _strtoi64(input.c_str(), nullptr, 10);

        }
    }
    case ERadix::Hex: return _strtoui64(input.c_str(), nullptr, 16);
    case ERadix::Binary: 
    {
        //stoi doesn't support the 0b prefix for binary values

        return _strtoui64(input.substr(2, input.length()-2).c_str(), nullptr, 2);
    }
    }

    return INFINITY;
}


std::string DoodadModule::IntToRadixString(inttype input, ERadix radix) const
{
    char output[1024];
    memset(output, 0, sizeof(output));

    int r = 10;
    if (radix == ERadix::Binary) r = 2;
    if (radix == ERadix::Hex) r = 16;

    if (input > 0)
    {
        inttype uinput = input;
        _ui64toa_s(uinput, output, sizeof(output), r);
    }
    else
    {
        _i64toa_s(input, output, sizeof(output), r);
    }
    std::string prefix = "";
    if (radix == ERadix::Binary) prefix = "0b";
    if (radix == ERadix::Hex) prefix = "0x";

    return prefix+ std::string(output);
}

ERadix DoodadModule::GetRadix(std::string input) const
{
    if (IsHex(input)) return ERadix::Hex;
    if (IsDecimal(input) || IsInteger(input)) return ERadix::Decimal;
    if (IsBinary(input)) return ERadix::Binary;
    return ERadix::Invalid;
}

bool DoodadModule::IsWhitespace(std::string input) const
{
    std::vector<std::string> whitespace;
    whitespace.push_back(" ");
    whitespace.push_back("\n");
    whitespace.push_back("\r\n");
    whitespace.push_back("\t");
    return std::find(whitespace.begin(), whitespace.end(), input) != whitespace.end();
}

bool DoodadModule::IsWhitespace(char input) const
{
    std::vector<std::string> whitespace;
    whitespace.push_back(" ");
    whitespace.push_back("\n");
    whitespace.push_back("\r\n");
    whitespace.push_back("\t");
    return std::find(whitespace.begin(), whitespace.end(), std::string(&input)) != whitespace.end();
}

std::vector<std::string> DoodadModule::TokenizeString(const std::string& source, std::vector<std::string> delimiters) const
{
    uint32_t iter = 0;

    std::vector<std::string> multiCharDelims;
    std::vector<char> singleCharDelims;
    for (uint32_t i = 0; i < delimiters.size(); ++i)
    {
        if (delimiters[i].length() > 1)
        {
            multiCharDelims.push_back(delimiters[i]);
        }
        else
        {
            singleCharDelims.push_back(delimiters[i].c_str()[0]);
        }
    }

    std::vector<char> whitespace;
    whitespace.push_back(' ');
    whitespace.push_back('\n');
    whitespace.push_back('\r\n');
    whitespace.push_back('\t');

    //first, tokenize based on single char delims and whitespace

    auto IsDelimeter = [](char c, std::vector<char> delimVec) -> bool
    {
        return std::find(delimVec.begin(), delimVec.end(), c) != delimVec.end();
    };


    std::vector<std::string> tokens;
    std::string word;

    while (iter < source.length())
    {
        char c = source[iter];
        if (IsDelimeter(c, singleCharDelims))
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

    //now split any tokens into multiple based on the multi char delims
    std::vector<std::string> finalTokens;

    for (int i = 0; i < tokens.size(); ++i)
    {
        bool found = false;

        for (int j = 0; j < multiCharDelims.size(); ++j)
        {
            int loc = tokens[i].find(multiCharDelims[j]);
            if (loc != std::string::npos)
            {
                int delimLen = multiCharDelims[j].length();
                std::string left = tokens[i].substr(0, loc);
                std::string mid = tokens[i].substr(loc, delimLen);
                std::string right = tokens[i].substr(loc + delimLen, tokens[i].length() - (loc + delimLen));

                if (left.length() > 0)
                    finalTokens.push_back(left);

                if (mid.length() > 0)
                    finalTokens.push_back(mid);

                if (right.length() > 0)
                    finalTokens.push_back(right);

                found = true;
                break;
            }
        }

        if (!found) finalTokens.push_back(tokens[i]);
    }
    for (int i = 0; i < finalTokens.size(); ++i)
    {
        finalTokens[i].erase(std::remove_if(finalTokens[i].begin(), finalTokens[i].end(), ::isspace), finalTokens[i].end());

    }
    return finalTokens;
}
