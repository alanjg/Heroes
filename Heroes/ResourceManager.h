#pragma once
class Faction;
class EntityFactory;
class MapTemplate;
class Renderer;
class Unit;
class UnitDefinition;
class EntityDescriptor;
class ResourceManager
{
	std::shared_ptr<Renderer> m_renderer;
	std::string dataRoot;
	void LoadFaction(const std::string& factionName);
	std::unordered_map<std::string, std::shared_ptr<Faction>> factions;

	std::unordered_map<std::string, std::shared_ptr<MapTemplate>> maps;
	std::shared_ptr<EntityFactory> entityFactory;
public:
	ResourceManager(std::shared_ptr<Renderer> renderer);
	~ResourceManager();

	void LoadResources();
	std::unordered_map<std::string, std::shared_ptr<Faction>>& GetFactions();
	EntityFactory& GetEntityFactory();
	std::shared_ptr<Unit> InstantiateUnit(std::shared_ptr<UnitDefinition> unitDefinition, float x, float y);
	std::shared_ptr<Unit> InstantiateUnit(std::shared_ptr<EntityDescriptor> entityDescriptor);
};

