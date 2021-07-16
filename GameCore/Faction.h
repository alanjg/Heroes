#pragma once

#include "EntityDefinition.h"
class Faction
{
public:
	Faction();
	~Faction();

	std::string name;
	std::vector<EntityDefinition*> units;
	std::vector<EntityDefinition*> structures;
};
