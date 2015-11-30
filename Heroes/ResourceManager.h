#pragma once
class Faction;
class Entity;
class EntityFactory;
class MapTemplate;
class Renderer;
struct EntityDefinition;
class EntityDescriptor;

class ResourceManager
{
	std::shared_ptr<Renderer> m_renderer;
	std::string m_dataRoot;
	void LoadFaction(const std::string& factionName);
	std::unordered_map<std::string, std::shared_ptr<Faction>> factions;

	std::unordered_map<std::string, std::shared_ptr<MapTemplate>> maps;
	std::shared_ptr<EntityFactory> entityFactory;
public:
	ResourceManager(std::shared_ptr<Renderer> renderer, const std::string& dataRoot);
	~ResourceManager();

	void LoadResources();
	std::unordered_map<std::string, std::shared_ptr<Faction>>& GetFactions();
	std::unordered_map<std::string, std::shared_ptr<MapTemplate>>& GetMaps();
	EntityFactory& GetEntityFactory();
	//std::shared_ptr<Entity> InstantiateEntity(std::shared_ptr<EntityDefinition> entityDefinition, float x, float y);
	std::shared_ptr<Entity> InstantiateEntity(std::shared_ptr<EntityDescriptor> entityDescriptor, Faction* faction);

};

