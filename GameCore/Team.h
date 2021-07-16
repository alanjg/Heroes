#pragma once
#include "Player.h"
struct Team
{
	int id;
	std::vector<Player*> players;
	std::vector<Team*> allies;
};

