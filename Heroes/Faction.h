#pragma once

class EntityFactory;
struct EntityDefinition;
class Faction
{
	std::shared_ptr<EntityFactory> entityFactory;
	std::vector<std::shared_ptr<EntityDefinition>> m_entityDefinitions;
public:
	Faction(std::shared_ptr<EntityFactory> factory);
	~Faction();

	std::string name;

	void LoadFromDirectory(std::string name, std::string directory);
	void LoadUnitFromFile(std::string fileName);
	void LoadStructureFromFile(std::string fileName);

	std::shared_ptr<EntityDefinition> ResolveAliasedEntityDefinition(const std::string& alias);
};
