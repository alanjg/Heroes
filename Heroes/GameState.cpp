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
	for each(auto player in players)
	{
		player->CheckElimination();
		if (player->GetIsEliminated())
		{
			eliminated++;
		}
	}
	return eliminated + 1 >= players.size();
}

void GameState::Update(float elapsedTime)
{
	for each(auto player in players)
	{
		for each(auto unit in player->GetUnits())
		{
			unit;
		}
	}
}