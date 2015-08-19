#pragma once
class Faction;
class ResourceManager
{
	std::string dataRoot;
	void LoadFaction(const std::string& factionName);
	std::unordered_map<std::string, std::shared_ptr<Faction>> factions;
public:
	ResourceManager();
	~ResourceManager();

	void LoadResources();
	std::unordered_map<std::string, std::shared_ptr<Faction>>& GetFactions();

};

