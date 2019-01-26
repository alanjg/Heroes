#include "stdafx.h"
#include "ResourceManager.h"
#include "File.h"
#include "Faction.h"
#include "EntityFactory.h"
#include "MapTemplate.h"
#include "Entity.h"
#include "EntityDefinition.h"
#include "Renderer.h"
#include "StaticMeshInstance.h"
#include "SkinnedMeshInstance.h"

ResourceManager::ResourceManager(std::shared_ptr<Renderer> renderer)
	:dataRoot("C:\\Users\\alanga\\Documents\\GitHub\\Heroes\\data\\"),
	m_renderer(renderer)
{
	entityFactory.reset(new EntityFactory());
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::LoadFaction(const std::string& factionName)
{
	std::shared_ptr<Faction> faction(new Faction(entityFactory));
	faction->LoadFromDirectory(factionName, dataRoot + factionName + "\\");
	factions[factionName] = faction;
}

void ResourceManager::LoadResources()
{
	std::ifstream infile(dataRoot + "factions.txt");
	std::string factionName;
	while (infile >> factionName)
	{
		LoadFaction(factionName);
	}

	std::ifstream mapsInfile(dataRoot + "maps.txt");
	std::string mapName;
	while (mapsInfile >> mapName)
	{
		std::shared_ptr<MapTemplate> map(new MapTemplate(entityFactory));
		map->LoadFromFile(dataRoot + "Maps\\" + mapName + ".txt");
		maps[mapName] = map;
	}
}

std::unordered_map<std::string, std::shared_ptr<Faction>>& ResourceManager::GetFactions()
{
	return factions;
}

std::unordered_map<std::string, std::shared_ptr<MapTemplate>>& ResourceManager::GetMaps()
{
	return maps;
}

EntityFactory& ResourceManager::GetEntityFactory()
{
	return *entityFactory.get();
}

/*
std::shared_ptr<Entity> ResourceManager::InstantiateEntity(std::shared_ptr<EntityDefinition> entityDefinition, float x, float y)
{
	SkinnedMeshInstance* mesh = m_renderer->CreateSkinnedMeshInstance(entityDefinition->modelName);

	std::shared_ptr<Entity> unit(new Entity());
	mesh->BindEntity(unit.get());
	unit->Initialize(x, y, mesh);
	return unit;
}
*/

std::shared_ptr<Entity> ResourceManager::InstantiateEntity(std::shared_ptr<EntityDescriptor> entityDescriptor, Faction* faction)
{
	std::string meshName;
	std::shared_ptr<EntityDefinition> entityDefinition;
	if (entityDescriptor->name.size() == 0)
	{
		entityDefinition = faction->ResolveAliasedEntityDefinition(entityDescriptor->alias);
	}
	else
	{
		entityDefinition = entityFactory->Lookup(entityDescriptor->name);
	}

	MeshInstance* mesh = m_renderer->CreateSkinnedMeshInstance(entityDefinition->modelName);
	if (mesh == nullptr)
	{
		mesh = m_renderer->CreateStaticMeshInstance(entityDefinition->modelName);
	}

	std::shared_ptr<Entity> unit(new Entity(*entityDefinition.get()));
	mesh->BindEntity(unit.get());
	unit->Initialize((float)entityDescriptor->startX, (float)entityDescriptor->startY, (float)entityDescriptor->orientation, mesh);
	return unit;
}