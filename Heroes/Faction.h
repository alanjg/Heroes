#pragma once

class EntityFactory;
class Faction
{
	std::shared_ptr<EntityFactory> entityFactory;
public:
	Faction(std::shared_ptr<EntityFactory> factory);
	~Faction();

	std::string name;

	void LoadFromDirectory(std::string name, std::string directory);
	void LoadUnitFromFile(std::string fileName);
	void LoadStructureFromFile(std::string fileName);
};
