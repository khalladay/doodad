#pragma once
#include <string>
#include <vector>
#include <stdint.h>


#define inttype int64_t 

enum class ERadix
{
    Decimal = 0,
    Hex,
    Binary,
    Invalid
};

class DoodadModule
{
public:
	virtual bool CanHandleInput(std::string input) const = 0;
	virtual std::string HandleInput(std::string input) const = 0;

protected:

    bool IsTokenSetValid(std::vector<std::string>& tokens) const;
    bool IsHex(std::string input) const;
    bool IsDecimal(std::string input) const;
    bool IsInteger(std::string input) const;
    bool IsBinary(std::string input) const;
    bool IsNumeric(std::string input) const;
    bool IsWhitespace(std::string input) const;
    bool IsWhitespace(char input) const;
    float DecimalToFloat(std::string input) const;
    inttype NumericToInt(std::string input) const;
    std::string IntToRadixString(inttype input, ERadix radix) const;
    ERadix GetRadix(std::string input) const;

    std::vector<std::string> TokenizeString(const std::string& source, std::vector<std::string> delimiters) const;
  };