#include "stdafx.h"
#include "Game.h"
#include "GameState.h"
#include "Entity.h"
#include "Map.h"
#include "Team.h"
#include "Player.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "SelectionManager.h"
#include "InputManager.h"
#include "InputPlayerController.h"
#include "AIPlayerController.h"

Game::Game(Renderer* renderer, SelectionManager* selectionManager, InputManager* inputManager, ResourceManager* resourceManager)
{
	m_renderer = renderer;
	m_selectionManager = selectionManager;
	m_inputManager = inputManager;
	m_resourceManager = resourceManager;
}

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
	std::shared_ptr<MapTemplate> mapTemplate = m_resourceManager->GetMaps()["Map1"];
	std::vector<std::shared_ptr<PlayerController>> playerControllers;
	playerControllers.push_back(std::shared_ptr<PlayerController>(new InputPlayerController()));
	playerControllers.push_back(std::shared_ptr<PlayerController>(new AIPlayerController()));
	map->Create(playerControllers, mapTemplate, m_resourceManager);
	state->SetMap(map);

	std::shared_ptr<Team> t1(new Team());
	std::shared_ptr<Player> p1(new Player());
	state->AddTeam(t1);
	t1->GetPlayers().push_back(p1);
	playerControllers[0]->SetPlayer(p1);

	std::shared_ptr<Team> t2(new Team());
	std::shared_ptr<Player> p2(new Player());
	state->AddTeam(t2);
	t2->GetPlayers().push_back(p2);
	playerControllers[1]->SetPlayer(p2);
}