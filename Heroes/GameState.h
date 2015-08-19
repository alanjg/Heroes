#pragma once

class Player;
class PlayerController;
class Entity;
class Structure;
class Map;
class Team;

class GameState
{
	std::shared_ptr<Map> m_map;
	std::vector<std::shared_ptr<Team>> m_teams;
	std::vector<std::shared_ptr<Player>> m_players;
	std::vector<std::shared_ptr<PlayerController>> m_playerControllers;
	std::vector<std::shared_ptr<Structure>> m_neutralStructures;

public:
	GameState();
	~GameState();

	bool CheckForGameEnd();
	void Update(float elapsedTime);

	void AddTeam(std::shared_ptr<Team> team);
	void AddPlayer(std::shared_ptr<Player> player);
	void AddNeutralStructure(std::shared_ptr<Structure> structure);
	void SetMap(std::shared_ptr<Map> map);
};

