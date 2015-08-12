#include "stdafx.h"
#include "Game.h"
#include "Entity.h"

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