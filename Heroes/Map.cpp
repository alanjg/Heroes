#include "stdafx.h"
#include "Map.h"
#include "MapTemplate.h"

Map::Map()
{

}

Map::~Map()
{
}

void Map::Create(std::vector<std::shared_ptr<PlayerController>>& playerControllers, MapTemplate& mapTemplate)
{
	players = playerControllers;
}