#pragma once
#include "doodad_view.h"
#include <string>
#include <vector>

class DoodadModule;

class DoodadController : public DoodadViewDelegate
{
public:

	DoodadController();
	~DoodadController();

	virtual std::string Eval(std::string input);

private:
	std::vector<DoodadModule*> mModules;
};
