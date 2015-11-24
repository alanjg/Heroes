#pragma once
class MapTemplate;
class Entity;
class PlayerController;
class ResourceManager;

class Map
{
	int sizeX, sizeY;
	std::vector<std::vector<bool>> mapNavigableCells;
	std::vector<std::vector<std::shared_ptr<Entity>>> cellOccupants;
	std::vector<std::shared_ptr<PlayerController>> m_playerControllers;
public:
	Map();
	~Map();
	void Create(std::vector<std::shared_ptr<PlayerController>> playerControllers, std::shared_ptr<MapTemplate> mapTemplate, ResourceManager* resourceManager);
};

