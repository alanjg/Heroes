#include "stdafx.h"
#include "MapTemplate.h"
#include "Entity.h"
#include "EntityFactory.h"
#include "../rapidjson/rapidjson.h"
#include "../rapidjson/filereadstream.h"
#include "../rapidjson/document.h"

MapTemplate::MapTemplate(std::shared_ptr<EntityFactory> factory)
{
	entityFactory = factory;
}

MapTemplate::~MapTemplate()
{
}

void MapTemplate::LoadFromFile(const std::string& fileName)
{
	FILE* file;
	fopen_s(&file, fileName.c_str(), "r");
	char readBuffer[2048];
	rapidjson::FileReadStream is(file, readBuffer, sizeof(readBuffer));
	rapidjson::Document doc;
	doc.ParseStream(is);

	name = doc["Name"].GetString();

	width = doc["Width"].GetInt();
	height = doc["Height"].GetInt();
	maxPlayers = doc["MaxPlayers"].GetInt();

	rapidjson::Value& playerStartLocations = doc["PlayerStartLocations"];
	for (unsigned int i = 0; i < playerStartLocations.Capacity(); i++)
	{
		std::shared_ptr<PlayerStartLocation> loc(new PlayerStartLocation());
		rapidjson::Value& location = playerStartLocations[i];
		loc->startX = location["CellX"].GetInt();
		loc->startY = location["CellY"].GetInt();

		rapidjson::Value& entities = location["Entities"];
		for (unsigned int j = 0; j < entities.Capacity(); j++)
		{
			rapidjson::Value& entity = entities[j];
			std::shared_ptr<EntityDescriptor> ed(new EntityDescriptor());
			if (entity.HasMember("Type"))
			{
				ed->name = entity["Type"].GetString();
			}
			else if (entity.HasMember("Alias"))
			{
				ed->alias = entity["Alias"].GetString();
			}
			else
			{
				continue;
			}
			loc->startX = entity["X"].GetInt();
			loc->startY = entity["Y"].GetInt();
			loc->entities.push_back(ed);
		}
		startLocations.push_back(loc);
	}

	rapidjson::Value& blockedCellValue = doc["BlockedCells"];
	for (unsigned int i = 0; i < blockedCellValue.Capacity(); i++)
	{
		std::pair<int, int> c;
		rapidjson::Value& cell = blockedCellValue[i];
		c.first = cell["X"].GetInt();
		c.second = cell["Y"].GetInt();
		blockedCells.push_back(c);
	}

	rapidjson::Value& sceneryValue = doc["Scenery"];
	for (unsigned int i = 0; i < sceneryValue.Capacity(); i++)
	{
	}

	rapidjson::Value& structuresValue = doc["Structures"];
	for (unsigned int i = 0; i < structuresValue.Capacity(); i++)
	{
		rapidjson::Value& structure = structuresValue[i];
		std::shared_ptr<EntityDescriptor> s(new EntityDescriptor());
		s->name = structure["Type"].GetString();
			
		s->startX = structure["X"].GetInt();
		s->startY = structure["Y"].GetInt();
		structures.push_back(s);
	}
}