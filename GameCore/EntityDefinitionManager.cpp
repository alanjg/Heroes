#include "pch.h"
#include "EntityDefinitionManager.h"
#include "ActionDefinition.h"
#include "../rapidjson/rapidjson.h"
#include "../rapidjson/document.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"
#include "../rapidjson/filereadstream.h"

EntityDefinitionManager::EntityDefinitionManager() :
	globalEntityDefinitionId(0)
{

}

void EntityDefinitionManager::Initialize(const std::string& dataRoot)
{
	std::ifstream infile(dataRoot + "factions.txt");
	std::string factionName;
	while (infile >> factionName)
	{
		Faction* faction = new Faction();
		LoadFactionDirectory(factionName, dataRoot + factionName + "\\", faction);
		m_factionLookup[factionName] = faction;
		m_factions.emplace_back(faction);
	}
}

void EntityDefinitionManager::LoadFactionDirectory(const std::string& name, const std::string& directory, Faction* faction)
{
	std::string fileName = directory + name + ".txt";
	FILE* file = nullptr;
	errno_t error = fopen_s(&file, fileName.c_str(), "r");
	if (file != nullptr)
	{
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
			LoadUnitFromFile(unitFileName, faction);
		}

		rapidjson::Value& structures = doc["Structures"];
		for (unsigned int i = 0; i < structures.Capacity(); i++)
		{
			rapidjson::Value& structure = structures[i];
			std::string structureName = structure.GetString();
			std::string structureFileName = directory + structureName + ".txt";
			LoadStructureFromFile(structureFileName, faction);
		}
		fclose(file);
	}
}

void EntityDefinitionManager::LoadUnitFromFile(const std::string& fileName, Faction* faction)
{
	FILE* file = nullptr;
	fopen_s(&file, fileName.c_str(), "r");
	if (file != nullptr)
	{
		char readBuffer[2048];
		rapidjson::FileReadStream is(file, readBuffer, sizeof(readBuffer));

		rapidjson::Document doc;
		doc.ParseStream(is);

		EntityDefinition* entity = new EntityDefinition();

		entity->name = doc["Name"].GetString();
		entity->alias = doc["Alias"].GetString();
		entity->modelName = doc["ModelName"].GetString();

		if (doc.HasMember("Width"))
		{
			entity->width = doc["Width"].GetDouble();
		}
		if (doc.HasMember("Height"))
		{
			entity->width = doc["Height"].GetDouble();
		}

		//units can move
		//entity->canMove = doc["CanMove"].GetBool();
		entity->canMove = true;


		entity->moveSpeed = doc["MoveSpeed"].GetDouble();

		entity->size = doc["Size"].GetDouble();
		
		// Melee attack
		entity->attackHitTime = doc["AttackHitTime"].GetDouble();
		entity->attackDuration = doc["AttackDuration"].GetDouble();
		entity->meleeAttackName = doc["MeleeAttackName"].GetString();

		entity->attackLeaveRange = doc["AttackLeaveRange"].GetDouble();
		entity->attackValue = doc["AttackValue"].GetInt();
		entity->damageHigh = doc["DamageHigh"].GetInt();
		entity->damageLow = doc["DamageLow"].GetInt();

		// Range attack
		entity->hasRangedAttack = doc["HasRangedAttack"].GetBool();
		if (entity->hasRangedAttack)
		{
			entity->rangedAttackRange = doc["RangedAttackRange"].GetDouble();
			entity->rangedAttackHitTime = doc["RangedAttackHitTime"].GetDouble();
			entity->rangedAttackDuration = doc["RangedAttackDuration"].GetDouble();
			entity->rangedAttackName = doc["RangedAttackName"].GetString();
		}
		else
		{
			entity->rangedAttackRange = 0;
			entity->rangedAttackHitTime = 0;
			entity->rangedAttackDuration = 0;
			entity->rangedAttackName = "";
		}

		entity->attackedDuration = doc["AttackedDuration"].GetDouble();
		entity->deathDuration = doc["DeathDuration"].GetDouble();

		entity->aggroRange = doc["AggroRange"].GetDouble();

		entity->defenseValue = doc["DefenseValue"].GetInt();
		entity->hitPoints = doc["HitPoints"].GetInt();

		entity->type = globalEntityDefinitionId++;


		/*if (doc.HasMember("SpecialActions"))
		{
			rapidjson::Value& actions = doc["Actions"];
			for (size_t i = 0; i < actions.Capacity(); i++)
			{
				rapidjson::Value& a = actions[i];
				ActionDefinition* action = new ActionDefinition();
				action->name = a["Name"].GetString();
				action->animation = a["Animation"].GetString();
				action->cooldown = a["Cooldown"].GetDouble();
				action->damageHigh = a["DamageHigh"].GetInt();
				action->damageLow = a["DamageLow"].GetInt();
				action->duration = a["Duration"].GetDouble();
				action->range = a["Range"].GetDouble();
				action->triggerTime = a["TriggerTime"].GetDouble();
				entity->specialActions.emplace_back(action);
			}
		}*/
		
		m_entityDefinitions.emplace_back(entity);
		faction->units.push_back(entity);
		fclose(file);
	}
}

void EntityDefinitionManager::LoadStructureFromFile(const std::string& fileName, Faction* faction)
{
	FILE* file;
	fopen_s(&file, fileName.c_str(), "r");
	if(file != nullptr)
	{
		char readBuffer[2048];
		rapidjson::FileReadStream is(file, readBuffer, sizeof(readBuffer));
		rapidjson::Document doc;
		doc.ParseStream(is);

		EntityDefinition* structure = new EntityDefinition();
		structure->name = doc["Name"].GetString();
		structure->alias = doc["Alias"].GetString();
		structure->modelName = doc["ModelName"].GetString();

		structure->hitPoints = doc["HitPoints"].GetInt();
		structure->width = doc["Width"].GetInt();
		structure->height = doc["Height"].GetInt();

		structure->type = globalEntityDefinitionId++;

		/*
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
		*/
		/*
		rapidjson::Value& production = doc["Production"];
		for (unsigned int i = 0; i < production.Capacity(); i++)
		{
			rapidjson::Value& p = production[i];
			std::string ps = p.GetString();
			EntityDefinition* entity = entityFactory->Lookup(ps);
			if (entity != nullptr)
			{
				structure->productionOptions.push_back(entity);
			}
		}
		*/
		m_entityDefinitions.emplace_back(structure);
		faction->structures.push_back(structure);

		fclose(file);
	}
}


EntityDefinition* EntityDefinitionManager::GetEntity(const std::string& name)
{
	return m_entityDefinitionLookup[name];
}

EntityDefinition* EntityDefinitionManager::GetEntity(int id)
{
	return m_entityDefinitions[id].get();
}

Faction* EntityDefinitionManager::GetFaction(const std::string& name)
{
	return m_factionLookup[name];
}

Faction* EntityDefinitionManager::GetFaction(int id)
{
	return m_factions[id].get();
}