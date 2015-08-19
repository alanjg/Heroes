#include "stdafx.h"
#include "ResourceManager.h"
#include "File.h"

ResourceManager::ResourceManager()
	:dataRoot("C:\\Users\\alanga\\Documents\\GitHub\\Heroes\\data\\")
{
}


ResourceManager::~ResourceManager()
{
}

void ResourceManager::LoadFaction(const std::string& factionName)
{

}

void ResourceManager::LoadResources()
{
	std::ifstream infile(dataRoot + "factions.txt");
	std::string factionName;
	while (infile >> factionName)
	{
		LoadFaction(factionName);
	}
}

std::unordered_map<std::string, std::shared_ptr<Faction>>& ResourceManager::GetFactions()
{
	return factions;
}