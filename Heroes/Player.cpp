#include "stdafx.h"
#include "Player.h"


Player::Player()
{
}


Player::~Player()
{
}


std::vector<std::shared_ptr<Unit>>& Player::GetUnits()
{
	return units;
}

std::vector<std::shared_ptr<Structure>>& Player::GetStructures()
{
	return structures;
}

void Player::CheckElimination()
{
	if (units.size() == 0 && structures.size() == 0)
	{
		isEliminated = true;
	}
}

bool Player::GetIsEliminated()
{
	return isEliminated;
}