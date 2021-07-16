#pragma once

#include "Cost.h"
#include "EntityAction.h"

struct EntityDefinition3
{
	EntityDefinition3();
	~EntityDefinition3();

	std::string name;
	std::string alias;
	std::string modelName;

	bool canMove;
	bool canAttack;
	float size;
	float speed;
	int attack;
	int defense;
	int hitPoints;

	int width;
	int height;
	int buildTime;
	Cost cost;

	std::vector<std::shared_ptr<EntityDefinition3>> productionOptions;
	std::vector<EntityActionType> actions;
};