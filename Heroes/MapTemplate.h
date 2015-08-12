#pragma once

#include "PlayerStartLocation.h"

class EntityFactory;

class MapTemplate
{
	std::shared_ptr<EntityFactory> entityFactory;
public:
	MapTemplate(std::shared_ptr<EntityFactory> factory);
	~MapTemplate();

	std::string name;
	int width, height;
	int maxPlayers;

	std::vector<std::shared_ptr<PlayerStartLocation>> startLocations;
	std::vector<std::pair<int, int>> blockedCells;
	std::vector<std::shared_ptr<EntityDescriptor>> structures;
	void LoadFromFile(const std::string& fileName);
};

