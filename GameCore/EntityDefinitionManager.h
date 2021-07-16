#pragma once
#include "EntityDefinition.h"
#include "Faction.h"
class EntityDefinitionManager
{
	int globalEntityDefinitionId;
	std::vector<std::unique_ptr<EntityDefinition>> m_entityDefinitions;
	std::unordered_map<std::string, EntityDefinition*> m_entityDefinitionLookup;
	std::vector<std::unique_ptr<Faction>> m_factions;
	std::unordered_map<std::string, Faction*> m_factionLookup;

	void LoadFactionDirectory(const std::string& name, const std::string& directory, Faction* faction);
	void LoadUnitFromFile(const std::string& fileName, Faction* faction);
	void LoadStructureFromFile(const std::string& fileName, Faction* faction);
public:
	EntityDefinitionManager();
	void Initialize(const std::string& dataRoot);

	EntityDefinition* GetEntity(const std::string& name);
	EntityDefinition* GetEntity(int id);
	Faction* GetFaction(const std::string& name);
	Faction* GetFaction(int id);
};

