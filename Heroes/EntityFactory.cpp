#include "stdafx.h"
#include "EntityFactory.h"


EntityFactory::EntityFactory()
{
}


EntityFactory::~EntityFactory()
{
}

void EntityFactory::Register(const std::string& name, std::shared_ptr<EntityDefinition> entity)
{
	entities[name] = entity;
}

EntityDefinition* EntityFactory::Lookup(const std::string& name)
{
	auto ptr = entities.find(name);
	if (ptr != entities.end())
	{
		return ptr->second.get();
	}
	else
	{
		return 0;
	}
}


std::shared_ptr<Entity> EntityFactory::Create(const std::string& name)
{
	std::shared_ptr<Entity> entity;
	return entity;
}