#include "module_debug.h"

bool ModuleDebug::CanHandleInput(std::string input) const
{


	return true;
}

std::string ModuleDebug::HandleInput(std::string input) const
{
	std::vector<std::string> delims;
	delims.push_back("+");
	delims.push_back(">>");
	delims.push_back("<<");
	delims.push_back("(");
	delims.push_back(")");

	std::vector<std::string> tokens = TokenizeString(input, delims);

	std::string output;
	for (auto& str : tokens)
	{
		output += str;

		if (IsDecimal(str))
		{
			output += " is decimal";
		}
		if (IsBinary(str))
		{
			output += " is binary";
		}

		if (IsHex(str))
		{
			output += " is hex";
		}

		output += "\n";
	}
	return output;
}
