#pragma once
#include "doodad_module.h"

class ModuleRadix: public DoodadModule
{
	virtual bool CanHandleInput(std::string input) const override;
	virtual std::string HandleInput(std::string input) const override;
};
