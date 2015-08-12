#pragma once
#include "Cost.h"
class ProductionOption
{
public:
	ProductionOption();
	virtual ~ProductionOption();

	std::string name;
	float time;
	Cost cost;
};

class UnitProductionOption : public ProductionOption
{
		
};