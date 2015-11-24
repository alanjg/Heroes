#include "stdafx.h"
#include "Map.h"
#include "MapTemplate.h"
#include "ResourceManager.h"
#include "PlayerController.h"
#include "Player.h"

Map::Map()
{

}

Map::~Map()
{
}

void Map::Create(std::vector<std::shared_ptr<PlayerController>> playerControllers, std::shared_ptr<MapTemplate> mapTemplate, ResourceManager* resourceManager)
{
	m_playerControllers = playerControllers;
	std::random_shuffle(mapTemplate->startLocations.begin(), mapTemplate->startLocations.end());
	for (unsigned int i = 0; i < m_playerControllers.size(); i++)
	{
		Player* player = m_playerControllers[i]->GetPlayer();
		PlayerStartLocation* startLocation = mapTemplate->startLocations[i].get();
		
		for each(auto entity in	startLocation->entities)
		{
			std::shared_ptr<Unit> unit = resourceManager->InstantiateUnit(entity);
			player->GetUnits().push_back(unit);
		}
	}
}