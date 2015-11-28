#include "stdafx.h"
#include "Player.h"


Player::Player()
{
}


Player::~Player()
{
}


std::vector<std::shared_ptr<Entity>>& Player::GetEntities()
{
	return m_entities;
}

void Player::CheckElimination()
{
	if (m_entities.size() == 0)
	{
		isEliminated = true;
	}
}

bool Player::GetIsEliminated()
{
	return isEliminated;
}

std::shared_ptr<Faction> Player::GetFaction()
{
	return m_faction;
}

void Player::SetFaction(std::shared_ptr<Faction> faction)
{
	m_faction = faction;
}