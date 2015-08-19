#include "stdafx.h"
#include "Team.h"


Team::Team()
{
}


Team::~Team()
{
}


std::vector<std::shared_ptr<Player>>& Team::GetPlayers()
{
	return players;
}