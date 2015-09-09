#include "stdafx.h"
#include "Faction.h"
#include "Unit.h"
#include "Structure.h"
#include "EntityFactory.h"
#include "../rapidjson/rapidjson.h"
#include "../rapidjson/document.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"
#include "../rapidjson/filereadstream.h"

Faction::Faction(std::shared_ptr<EntityFactory> factory)
{
	entityFactory = factory;
}

Faction::~Faction()
{
}

void Faction::LoadFromDirectory(std::string name, std::string directory)
{
	std::string fileName = directory + name + ".txt";
	FILE* file;
	errno_t error = fopen_s(&file, fileName.c_str(), "r");
	char readBuffer[2048];
	rapidjson::FileReadStream is(file, readBuffer, sizeof(readBuffer));

	rapidjson::Document doc;
	doc.ParseStream(is);
	rapidjson::Value& v = doc["Name"];
	bool iss = v.IsString();
	std::string factionName = doc["Name"].GetString();
	rapidjson::Value& units = doc["Units"];
	for (unsigned int i = 0; i < units.Capacity(); i++)
	{
		rapidjson::Value& unit = units[i];
		std::string unitName = unit.GetString();
		std::string unitFileName = directory + unitName + ".txt";
		LoadUnitFromFile(unitFileName);
	}

	rapidjson::Value& structures = doc["Structures"];
	for (unsigned int i = 0; i < structures.Capacity(); i++)
	{
		rapidjson::Value& structure = structures[i];
		std::string structureName = structure.GetString();
		std::string structureFileName = directory + structureName + ".txt";
		LoadStructureFromFile(structureFileName);
	}
}

void Faction::LoadUnitFromFile(std::string fileName)
{
	FILE* file;
	fopen_s(&file, fileName.c_str(), "r");
	char readBuffer[2048];
	rapidjson::FileReadStream is(file, readBuffer, sizeof(readBuffer));

	rapidjson::Document doc;
	doc.ParseStream(is);

	std::shared_ptr<UnitDefinition> entity(new UnitDefinition());
	entity->name = doc["Name"].GetString();
	entity->alias = doc["Alias"].GetString();
	entity->modelName = doc["ModelName"].GetString();
	entity->size = doc["Size"].GetDouble();
	entity->speed = doc["Speed"].GetDouble();
	entity->attack = doc["Attack"].GetInt();
	entity->defense = doc["Defense"].GetInt();
	entity->hitPoints = doc["HitPoints"].GetInt();
	entityFactory->Register(entity->name, entity);
}

void Faction::LoadStructureFromFile(std::string fileName)
{
	FILE* file;
	fopen_s(&file, fileName.c_str(), "r");
	char readBuffer[2048];
	rapidjson::FileReadStream is(file, readBuffer, sizeof(readBuffer));
	rapidjson::Document doc;
	doc.ParseStream(is);

	std::shared_ptr<StructureDefinition> structure(new StructureDefinition());
	structure->name = doc["Name"].GetString();
	structure->alias = doc["Alias"].GetString();
	structure->modelName = doc["ModelName"].GetString();
	structure->hitPoints = doc["HitPoints"].GetInt();
	structure->buildTime = doc["BuildTime"].GetDouble();
	structure->width = doc["Width"].GetInt();
	structure->height = doc["Height"].GetInt();
	rapidjson::Value& cost = doc["Cost"];
	if (cost.HasMember("Gold"))
	{
		structure->cost.Gold = cost["Gold"].GetInt();
	}
	if (cost.HasMember("Wood"))
	{
		structure->cost.Wood = cost["Wood"].GetInt();
	}
	if (cost.HasMember("Ore"))
	{
		structure->cost.Ore = cost["Ore"].GetInt();
	}
	if (cost.HasMember("Gems"))
	{
		structure->cost.Gems = cost["Gems"].GetInt();
	}

	rapidjson::Value& production = doc["Production"];
	for (unsigned int i = 0; i < production.Capacity(); i++)
	{
		rapidjson::Value& p = production[i];
		std::string ps = p.GetString();
		std::shared_ptr<EntityDefinition> entity = entityFactory->Lookup(ps);
		if (entity != 0)
		{
			structure->productionOptions.push_back(entity);
		}
	}

	entityFactory->Register(structure->name, structure);
}