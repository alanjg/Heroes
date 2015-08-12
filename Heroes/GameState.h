#pragma once

class Player;
class PlayerController;
class Entity;
class Structure;
class Map;

class GameState
{
	std::shared_ptr<Map> map;
	std::vector<std::shared_ptr<Player>> players;
	std::vector<std::shared_ptr<PlayerController>> playerControllers;
	std::vector<std::shared_ptr<Structure>> neutralStructures;

public:
	GameState();
	~GameState();

	bool CheckForGameEnd();
	void Update(float elapsedTime);
};

