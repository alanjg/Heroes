#include "stdafx.h"
#include "Game.h"
#include "GameState.h"
#include "Entity.h"
#include "Map.h"
#include "Team.h"
#include "Player.h"

void Game::Update(float elapsedTime)
{
	for each(auto entity in entities)
	{
		entity->Update(elapsedTime);
	}
}

void Game::Render()
{
	for each(auto entity in entities)
	{
		entity->Render();
	}
}


void Game::CreateTestGame()
{
	state.reset(new GameState());
	std::shared_ptr<Map> map(new Map());
	state->SetMap(map);

	std::shared_ptr<Team> t1(new Team());
	std::shared_ptr<Player> p1(new Player());
	state->AddTeam(t1);
	t1->GetPlayers().push_back(p1);

	std::shared_ptr<Team> t2(new Team());
	std::shared_ptr<Player> p2(new Player());
	state->AddTeam(t2);
	t2->GetPlayers().push_back(p2);
}