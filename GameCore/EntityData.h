#pragma once
#include "EntityDefinition.h"
#include "MapGrid.h"

enum class EntityStatus
{
	IDLE = 0,
	MOVING = 1,
	ATTACKING = 2,
	RANGE_ATTACKING = 3,
	ATTACKED = 4,
	SPECIAL = 5, // unimplemented
	DYING = 6,
	DEAD = 7,
};

enum class EntityCommand
{
	NONE = 0,
	MOVE_TO_POINT = 1,
	FOLLOW_UNIT = 2, // unimplemented
	ATTACK_UNIT = 3,
	ATTACK_POINT = 4,
	STOP = 5,
	HOLD = 6,
	SPECIAL = 7 // unimplemented
};

struct EntityCommandData
{
	int targetEntityId;
	float targetX, targetY, targetZ;
	int specialType;
};

struct EntityData
{
	EntityDefinition* entityDefinition;
	int playerId;
	int entityId;
	float orientation;
	float position[3];
	float velocity[3];

	EntityStatus status;
	float targetPointX, targetPointY, targetPointZ;
	int attackTarget;
	
	EntityCommand command;
	EntityCommandData commandData;
	
	// Time through the current action(based on status).
	float actionTime;
	
	int hitPoints;
	std::unique_ptr<Path> currentPath;
};