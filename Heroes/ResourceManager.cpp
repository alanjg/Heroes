#include "stdafx.h"
#include "ResourceManager.h"
#include "File.h"
#include "Faction.h"
#include "EntityFactory.h"
#include "MapTemplate.h"
#include "Unit.h"
#include "EntityDefinition.h"
#include "Renderer.h"
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
	while (infile >> mapName)
	{
		std::shared_ptr<MapTemplate> map(new MapTemplate(entityFactory));
		map->LoadFromFile(dataRoot + mapName + "\\");
		maps[mapName] = map;
	}
}

std::unordered_map<std::string, std::shared_ptr<Faction>>& ResourceManager::GetFactions()
{
	return factions;
}

EntityFactory& ResourceManager::GetEntityFactory()
{
	return *entityFactory.get();
}


std::shared_ptr<Unit> ResourceManager::InstantiateUnit(std::shared_ptr<UnitDefinition> unitDefinition, float x, float y)
{
	SkinnedMeshInstance* mesh = m_renderer->CreateSkinnedMeshInstance(unitDefinition->modelName);

	std::shared_ptr<Unit> unit(new Unit());
	mesh->BindEntity(unit.get());
	unit->Initialize(x, y, mesh);
	return unit;
}


std::shared_ptr<Unit> ResourceManager::InstantiateUnit(std::shared_ptr<EntityDescriptor> entityDescriptor)
{
	SkinnedMeshInstance* mesh = m_renderer->CreateSkinnedMeshInstance(entityDescriptor->name);

	std::shared_ptr<Unit> unit(new Unit());
	mesh->BindEntity(unit.get());
	unit->Initialize(entityDescriptor->startX, entityDescriptor->startY, mesh);
	return unit;

}