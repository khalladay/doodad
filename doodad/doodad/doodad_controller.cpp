#include "doodad_controller.h"
#include "common_header.h"
#include "module_ascii.h"
#include "module_bitwise.h"
#include "module_calculator.h"
#include "module_comment.h"
#include "module_radix.h"

DoodadController::DoodadController()
{
	mModules.push_back(new ModuleCalculator());
	mModules.push_back(new ModuleBitwise());
	mModules.push_back(new ModuleRadix());
	mModules.push_back(new ModuleAscii());
	mModules.push_back(new ModuleComment());
}

DoodadController::~DoodadController()
{

}

std::string DoodadController::Eval(std::string input)
{
	for (const auto* module : mModules)
	{
		if (module->CanHandleInput(input))
		{
			return module->HandleInput(input);
		}
	}

	return "";
}
