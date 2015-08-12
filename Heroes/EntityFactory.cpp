#include "stdafx.h"
#include "EntityFactory.h"


EntityFactory::EntityFactory()
{
}


EntityFactory::~EntityFactory()
{
}

void EntityFactory::Register(const std::string& name, std::shared_ptr<Entity> entity)
{
	entities[name] = entity;
}

std::shared_ptr<Entity> EntityFactory::Lookup(const std::string& name)
{
	auto ptr = entities.find(name);
	if (ptr != entities.end())
	{
		return ptr->second;
	}
	else
	{
		return 0;
	}
}