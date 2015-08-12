#pragma once
#include "Faction.h"

class Entity;
class Structure;
class Unit;

class Player
{
	bool isEliminated;
	std::vector<std::shared_ptr<Unit>> units;
	std::vector<std::shared_ptr<Structure>> structures;

	std::shared_ptr<Faction> faction;

public:
	Player();
	~Player();

	std::vector<std::shared_ptr<Unit>>& GetUnits();
	std::vector<std::shared_ptr<Structure>>& GetStructures();
	void CheckElimination();
	bool GetIsEliminated();
};

