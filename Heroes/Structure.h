#pragma once
#include "Entity.h"
#include "Cost.h"
class ProductionOption;

class Structure : public Entity
{
public:
	Structure();
	~Structure();

	int width;
	int height;
	int hitPoints;
	int buildTime;
	Cost cost;

	std::vector<std::shared_ptr<Entity>> productionOptions;
};

