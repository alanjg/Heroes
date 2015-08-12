#pragma once
class MapTemplate;
class Entity;
class PlayerController;

class Map
{
	int sizeX, sizeY;
	std::vector<std::vector<bool>> mapNavigableCells;
	std::vector<std::vector<std::shared_ptr<Entity>>> cellOccupants;
	std::vector<std::shared_ptr<PlayerController>> players;
public:
	Map();
	~Map();
	void Create(std::vector<std::shared_ptr<PlayerController>>& playerControllers, MapTemplate& mapTemplate);
};

