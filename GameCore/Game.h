#pragma once
#include "Player.h"
#include "Team.h"
#include "EntityDefinition.h"
#include "EntityData.h"
#include "EntityDefinitionManager.h"
#include "entity.grpc.pb.h"
#include "MapGrid.h"

class Game
{
public:
	Game();
	void LoadEntityDefinitions();


	int AddPlayer(const std::string& name);
	int CreateTeam(int playerId);
	int CreateTeam(const std::vector<int>& playerIds);
	void SetAlly(int team1, int team2);
	void CreateGame1();

	void UpdateGame(float elapsedTime);
	void ProcessCommand(int entityId, EntityCommand command, EntityCommandData commandData);
	void WriteServerState(heroes::ServerState& state);
	
	std::vector<std::unique_ptr<Player>> players;
private:
	void ApplyCommandToStatus(EntityData* entity);
	void PathEntityToPoint(EntityData* entity, float x, float y, float z);
	bool CanSeeUnit(EntityData* entity, EntityData* target);
	EntityData* GetClosestEnemyWithinAggroRange(EntityData* entity);
	bool IsHostile(EntityData* entity, EntityData* target);

	// Used to generate new ids.  Starts at zero and indexes the various lookups.
	int globalPlayerId;
	int globalTeamId;
	int globalEntityId;

	std::unordered_map<int, Player*> playerIdLookup;
	std::unordered_map<std::string, Player*> playerNameLookup;
	std::vector<std::unique_ptr<EntityData>> entities;
	std::vector<std::unique_ptr<EntityDefinition>> entityDefinitions;
	std::unique_ptr<EntityDefinitionManager> entityDefinitionManager;
	std::vector<std::unique_ptr<Team>> teams;
	std::unordered_map<int, Team*> playerIdToTeamLookup;
	std::unique_ptr<MapGrid> mapGrid;
};

