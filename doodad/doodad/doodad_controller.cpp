#include "doodad_controller.h"
#include "common_header.h"
#include "module_ascii.h"
#include "module_integer_calculator.h"
#include "module_calculator.h"
#include "module_comment.h"
#include "module_radix.h"
#include "module_debug.h"

DoodadController::DoodadController()
{
	//mModules.push_back(new ModuleDebug());
	mModules.push_back(new ModuleCalculator());
	mModules.push_back(new ModuleIntegerCalculator());
	mModules.push_back(new ModuleRadix());
	mModules.push_back(new ModuleAscii());
	mModules.push_back(new ModuleComment());
}

DoodadController::~DoodadController()
{

}

std::string DoodadController::Eval(std::string input)
{
	input.erase(std::remove_if(input.begin(), input.end(), ::isspace), input.end());

	for (const auto* module : mModules)
	{
		if (module->CanHandleInput(input))
		{
			return module->HandleInput(input);
		}
	}

	return "";
}
