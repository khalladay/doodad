#include "module_radix.h"

bool ModuleRadix::CanHandleInput(std::string input) const
{
	std::vector<std::string> delims;
	delims.push_back(" ");

	std::vector<std::string> tokens = TokenizeString(input, delims);

	if (tokens.size() != 1)
	{
		return false;
	}

	return IsInteger(tokens[0]) || IsHex(tokens[0]) || IsBinary(tokens[0]);
}

std::string ModuleRadix::HandleInput(std::string input) const
{
	ERadix r = GetRadix(input);

	std::string output = "";
	inttype value = NumericToInt(input);
	if (r == ERadix::Decimal)
	{
		output += "in hex: " + IntToRadixString(value, ERadix::Hex) + ". in binary: " + IntToRadixString(value, ERadix::Binary) + ".";
	}
	else if (r == ERadix::Binary)
	{
		output += "in decimal: " + IntToRadixString(value, ERadix::Decimal) + ". in hex: " + IntToRadixString(value, ERadix::Hex) + ".";
	}
	else if (r == ERadix::Hex)
	{
		output += "in decimal: " + IntToRadixString(value, ERadix::Decimal) + ". in binary: " + IntToRadixString(value, ERadix::Binary) + ".";
	}

	return input + ": " + output;
}