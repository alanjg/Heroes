#pragma once
#include "../GameCore/Faction.h"

class Entity;
class Structure;
class Team;

class Player
{
	bool isEliminated;
	std::vector<std::shared_ptr<Entity>> m_entities;

	std::shared_ptr<Faction> m_faction;
	std::shared_ptr<Team> team;

public:
	Player();
	~Player();

	std::vector<std::shared_ptr<Entity>>& GetEntities();
	std::shared_ptr<Faction> GetFaction();
	void SetFaction(std::shared_ptr<Faction> faction);
	void CheckElimination();
	bool GetIsEliminated();
};

