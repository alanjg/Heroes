#pragma once
#include "Entity.h"

struct EntityType
{
	int id;
	std::string name;
};

class EntityFactory
{
	std::unordered_map<std::string, std::shared_ptr<EntityDefinition>> entities;
public:
	EntityFactory();
	~EntityFactory();

	void Register(const std::string& name, std::shared_ptr<EntityDefinition> entity);
	std::shared_ptr<EntityDefinition> Lookup(const std::string& name);

	std::shared_ptr<Entity> Create(const std::string& name);
	std::shared_ptr<Entity> Create(int id);
	std::shared_ptr<Entity> Create(const EntityType& entityType);

	EntityType RegisterCallback(const std::string& name);
};

