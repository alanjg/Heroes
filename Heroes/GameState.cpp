#include "stdafx.h"
#include "GameState.h"
#include "Player.h"

GameState::GameState()
{
}

GameState::~GameState()
{
}

bool GameState::CheckForGameEnd()
{
	unsigned int eliminated = 0;
	for each(auto player in m_players)
	{
		player->CheckElimination();
		if (player->GetIsEliminated())
		{
			eliminated++;
		}
	}
	return eliminated + 1 >= m_players.size();
}

void GameState::Update(float elapsedTime)
{
	for each(auto player in m_players)
	{
		for each(auto entity in player->GetEntities())
		{
			entity;
		}
	}
}


void GameState::AddTeam(std::shared_ptr<Team> team)
{
	m_teams.push_back(team);
}

void GameState::AddPlayer(std::shared_ptr<Player> player)
{
	m_players.push_back(player);
}

void GameState::AddNeutralStructure(std::shared_ptr<Structure> structure)
{
	m_neutralStructures.push_back(structure);
}

void GameState::SetMap(std::shared_ptr<Map> map)
{
	m_map = map;
}