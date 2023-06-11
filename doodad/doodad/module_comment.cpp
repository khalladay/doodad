#include "module_comment.h"

bool ModuleComment::CanHandleInput(std::string input) const
{
	return true;
}

std::string ModuleComment::HandleInput(std::string input) const
{
	return input;
}