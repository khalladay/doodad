#include "module_ascii.h"
#include <vector>

bool ModuleAscii::CanHandleInput(std::string input) const 
{
	std::vector<std::string> tokens = TokenizeString(input, "\'");
	
	if (tokens.size() != 3)
	{
		return false;
	}

	if (tokens.size() == 3)
	{
		return tokens[1].length() == 1;
	}

	return false;
}

std::string ModuleAscii::HandleInput(std::string input) const
{
	std::vector<std::string> tokens = TokenizeString(input, "\'");

	std::string t = tokens[1];
	
	int v = (int)t[0];
	char ascii[32];
	char output[32];

	_itoa_s((int)t.c_str()[0], ascii, 10);
	
	sprintf_s(output, " \'%s\' : ascii value %s", t.c_str(), ascii);
	return output;
}
