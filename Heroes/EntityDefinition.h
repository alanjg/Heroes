#pragma once
#include "Cost.h"
class EntityDefinition
{
public:
	EntityDefinition();
	~EntityDefinition();

	std::string name;
	std::string alias;
	std::string modelName;
};

class UnitDefinition : public EntityDefinition
{
public:
	float size;
	float speed;
	int attack;
	int defense;
	int hitPoints;
};

class StructureDefinition : public EntityDefinition
{
public:
	int width;
	int height;
	int hitPoints;
	int buildTime;
	Cost cost;

	std::vector<std::shared_ptr<EntityDefinition>> productionOptions;
};