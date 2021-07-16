#include "pch.h"
#include "Game.h"
#include "Faction.h"



Game::Game()
{
	globalPlayerId = 0;
	globalTeamId = 0;
	globalEntityId = 0;
}

void Game::LoadEntityDefinitions()
{
	entityDefinitionManager.reset(new EntityDefinitionManager());
	entityDefinitionManager->Initialize("C:\\Users\\alan_\\Documents\\GitHub\\Heroes\\data\\");

	//// todo - load from file
	//EntityDefinition* entityDefinition = new EntityDefinition();
	//entityDefinition->aggroRange = 15;	
	//entityDefinition->attackDuration = 2.1f;
	//entityDefinition->attackLeaveRange = 0.3f;
	//entityDefinition->attackedDuration = 1.6f;
	//entityDefinition->attackHitTime = 1.7f;
	//entityDefinition->attackValue = 5;
	//entityDefinition->damageHigh = 6;
	//entityDefinition->damageLow = 3;
	//entityDefinition->deathDuration = 2;
	//entityDefinition->defenseValue = 3;
	//entityDefinition->hitPoints = 30;
	//entityDefinition->moveSpeed = 4;
	//entityDefinition->name = "Footman";
	//entityDefinition->size = 2;
	//entityDefinition->type = 0;
	//entityDefinitions.emplace_back(entityDefinition);

	//entityDefinition = new EntityDefinition();
	//entityDefinition->aggroRange = 15;
	//entityDefinition->attackDuration = 2;
	//entityDefinition->attackLeaveRange = 0.3f;
	//entityDefinition->attackedDuration = 1.8f;
	//entityDefinition->attackHitTime = 1.5f;
	//entityDefinition->attackValue = 5;
	//entityDefinition->damageHigh = 6;
	//entityDefinition->damageLow = 3;
	//entityDefinition->deathDuration = 2;
	//entityDefinition->defenseValue = 3;
	//entityDefinition->hitPoints = 30;
	//entityDefinition->moveSpeed = 6;
	//entityDefinition->name = "Demon";
	//entityDefinition->size = 2;
	//entityDefinition->type = 1;
	//entityDefinitions.emplace_back(entityDefinition);

	//entityDefinition = new EntityDefinition();
	//entityDefinition->aggroRange = 15;
	//entityDefinition->attackDuration = 4;
	//entityDefinition->attackLeaveRange = 0.3f;
	//entityDefinition->attackedDuration = 0.4f;
	//entityDefinition->attackHitTime = 0.5;
	//entityDefinition->attackValue = 5;
	//entityDefinition->damageHigh = 6;
	//entityDefinition->damageLow = 3;
	//entityDefinition->deathDuration = 2;
	//entityDefinition->defenseValue = 3;
	//entityDefinition->hitPoints = 10;
	//entityDefinition->moveSpeed = 2;
	//entityDefinition->name = "BlackDragon";
	//entityDefinition->size = 2.0;
	//entityDefinition->type = 2;
	//entityDefinitions.emplace_back(entityDefinition);
}

int Game::AddPlayer(const std::string& name)
{
	Player* player = new Player();
	player->id = globalPlayerId++;
	player->name = name;
	players.emplace_back(player);
	return player->id;
}

int Game::CreateTeam(int playerId)
{
	std::vector<int> playerIds;
	playerIds.push_back(playerId);
	return CreateTeam(playerIds);
}

int Game::CreateTeam(const std::vector<int>& playerIds)
{
	Team* team = new Team();
	team->id = globalTeamId++;
	for (auto it = playerIds.begin(); it != playerIds.end(); it++)
	{
		team->players.push_back(players[*it].get());
		playerIdToTeamLookup[*it] = team;
	}
	teams.emplace_back(team);
	return team->id;
}

void Game::SetAlly(int team1, int team2)
{
	Team* t1 = teams[team1].get();
	Team* t2 = teams[team2].get();
	t1->allies.push_back(t2);
	t2->allies.push_back(t1);
}

void Game::CreateGame1()
{
	// add computer player
	Player* player = new Player();
	int computerId = AddPlayer("Computer");
	CreateTeam(computerId);

	for (auto it = players.begin(); it != players.end(); it++)
	{
		Faction* faction = entityDefinitionManager->GetFaction((*it)->id);

		float x = 0;
		for (unsigned int j = 0; j < faction->units.size(); j++)
		{
			// Create 10 units for each player
			int unitCount = 10;
			for (int k = 0; k < unitCount; k++)
			{
				EntityData* entity = new EntityData();
				entity->entityId = globalEntityId++;

				//entity->entityDefinition = entityDefinitions[(*it)->id % 3].get();
				//entity->entityDefinition = entityDefinitionManager->GetEntity((*it)->id % 3);
				entity->entityDefinition = faction->units[j];
				entity->actionTime = 0;
				entity->status = EntityStatus::IDLE;
				entity->command = EntityCommand::NONE;
				entity->hitPoints = entity->entityDefinition->hitPoints;
				entity->orientation = 3.14f / 2 * (*it)->id;
				entity->playerId = (*it)->id;
				entity->position[0] = x;
				entity->position[1] = 0;
				entity->position[2] = (*it)->id * 25.0f;
				entities.emplace_back(entity);
				x += entity->entityDefinition->size * 2.1f;
			}

			x += 3;
		}
	}
	mapGrid.reset(new MapGrid(-10, -10, 110, 30));
}

bool Game::IsHostile(EntityData* entity, EntityData* target)
{
	Team* sourceTeam = playerIdToTeamLookup[entity->playerId];
	Team* targetTeam = playerIdToTeamLookup[target->playerId];
	// Hostile if the target player's team is not allied with source player's team
	return sourceTeam != targetTeam &&
		std::find(sourceTeam->allies.begin(), sourceTeam->allies.end(), targetTeam) == sourceTeam->allies.end();
}

bool Game::CanSeeUnit(EntityData* entity, EntityData* target)
{
	return true;
}

// sets current status to idle and clears velocity.  Clears current command.
void StopEntity(EntityData* entity)
{
	entity->status = EntityStatus::IDLE;
	entity->command = EntityCommand::NONE;

	entity->velocity[0] = 0;
	entity->velocity[1] = 0;
	entity->velocity[2] = 0;
}

// Returns true if entity is within distance of the target coordinates.  Writes dx/dy/dz as a normalized vector to the target
bool CheckRange(EntityData* entity, float x, float y, float z, float distance, float& dx, float& dy, float& dz, float& dist)
{
	dx = x - entity->position[0];
	dy = y - entity->position[1];
	dz = z - entity->position[2];
	dist = sqrt(dx * dx + dy * dy + dz * dz);
	dx /= dist;
	dy /= dist;
	dz /= dist;
	return dist <= distance;
}

void Game::PathEntityToPoint(EntityData* entity, float x, float y, float z)
{
	// todo - hit detection
	entity->targetPointX = x;
	entity->targetPointY = y;
	entity->targetPointZ = z;
}

void SetOrientationToNormalizedVector(EntityData* entity, float vx, float vy, float vz)
{
	entity->orientation = atan2(-vz, vx);
}

// Returns true if move was set, false if reached destination
bool SetMoveToTargetPoint(EntityData* entity, float x, float y, float z, float stopDistance)
{
	float dx = x - entity->position[0];
	float dy = y - entity->position[1];
	float dz = z - entity->position[2];
	float mag = sqrt(dx * dx + dy * dy + dz * dz);
	if (mag <= stopDistance)
	{
		// reached destination
		entity->velocity[0] = 0;
		entity->velocity[1] = 0;
		entity->velocity[2] = 0;
		return false;
	}
	else
	{
		entity->velocity[0] = dx / mag * entity->entityDefinition->moveSpeed;
		entity->velocity[1] = dy / mag * entity->entityDefinition->moveSpeed;
		entity->velocity[2] = dz / mag * entity->entityDefinition->moveSpeed;
		SetOrientationToNormalizedVector(entity, dx, dy, dz);
		return true;
	}
}

// return true if enemy killed
bool DoAttack(EntityData* attacker, EntityData* defender)
{
	int modifier = attacker->entityDefinition->attackValue - defender->entityDefinition->defenseValue;
	float adjust = powf(1.05f, (float)modifier);
	int result = rand() % (attacker->entityDefinition->damageHigh - attacker->entityDefinition->damageLow + 1);
	int adjustedDamage = int(ceil((result + attacker->entityDefinition->damageLow) * adjust));
	defender->hitPoints -= adjustedDamage;
	if (defender->hitPoints <= 0)
	{
		defender->hitPoints = 0;
		return true;
	}
	return false;
}

EntityData* Game::GetClosestEnemyWithinAggroRange(EntityData* entity)
{
	EntityData* closest = nullptr;
	float closestDistance = 1e9;
	for (auto targetIt = entities.begin(); targetIt != entities.end(); targetIt++)
	{
		EntityData* target = targetIt->get();
		
		if (IsHostile(entity, target) && CanSeeUnit(entity, target) && target->status != EntityStatus::DEAD && target->status != EntityStatus::DYING)
		{
			float dx, dy, dz, dist;
			if (CheckRange(entity, target->position[0], target->position[1], target->position[2],
				entity->entityDefinition->size + target->entityDefinition->size + entity->entityDefinition->aggroRange, dx, dy, dz, dist))
			{
				if (closest == nullptr || closestDistance > dist)
				{
					closest = target;
					closestDistance = dist;
				}
			}
		}
	}
	return closest;
}

// Applies the current command to entity, overriding status as needed.
void Game::ApplyCommandToStatus(EntityData* entity)
{
	if (entity->status == EntityStatus::ATTACKED || entity->status == EntityStatus::DYING || entity->status == EntityStatus::DEAD)
	{
		// Cannot apply command in these cases
		return;
	}

	if (entity->command == EntityCommand::MOVE_TO_POINT)
	{
		float dx, dy, dz, dist;
		if (CheckRange(entity, entity->commandData.targetX, entity->commandData.targetY, entity->commandData.targetZ,
			entity->entityDefinition->size, dx, dy, dz, dist))
		{
			// Reached destination, clear command.
			StopEntity(entity);
			// todo - waypoints
		}
		else
		{
			entity->status = EntityStatus::MOVING;
			PathEntityToPoint(entity, entity->commandData.targetX, entity->commandData.targetY, entity->commandData.targetZ);
			// todo: Check pathing here.
		}
	}
	else if (entity->command == EntityCommand::FOLLOW_UNIT)
	{
		// todo
	}
	else if (entity->command == EntityCommand::ATTACK_UNIT)
	{
		EntityData* target = entities[entity->commandData.targetEntityId].get();
		if (target->status == EntityStatus::DYING || target->status == EntityStatus::DEAD)
		{
			// target is dead, clear command.
			StopEntity(entity);
		}
		else if (!CanSeeUnit(entity, target))
		{
			// Out of sight, lost target, clear command
			StopEntity(entity);
		}
		else
		{
			float attackRange = entity->entityDefinition->size + target->entityDefinition->size;
			if (entity->status == EntityStatus::ATTACKING && entity->attackTarget == target->entityId)
			{
				attackRange += entity->entityDefinition->attackLeaveRange;
			}
			float dx, dy, dz, dist;
			if (CheckRange(entity, entity->targetPointX, entity->targetPointY, entity->targetPointZ,
				attackRange, dx, dy, dz, dist))
			{
				// In range of target, set attack status if not already set.
				if (entity->status != EntityStatus::ATTACKING || entity->attackTarget != target->entityId)
				{					
					entity->status = EntityStatus::ATTACKING;
					entity->attackTarget = target->entityId;
					entity->actionTime = 0;
				}
			}
			else
			{
				entity->status = EntityStatus::MOVING;
				PathEntityToPoint(entity, target->position[0], target->position[1], target->position[2]);
			}
		}
	}
	else if (entity->command == EntityCommand::ATTACK_POINT)
	{
		if (entity->status == EntityStatus::ATTACKING)
		{
			EntityData* target = entities[entity->attackTarget].get();
			float attackRange = entity->entityDefinition->size + target->entityDefinition->size + entity->entityDefinition->attackLeaveRange;
			float dx, dy, dz, dist;
			if (CheckRange(entity, target->position[0], target->position[1], target->position[2],
				attackRange, dx, dy, dz, dist))
			{
				if (target->status == EntityStatus::DEAD || target->status == EntityStatus::DYING)
				{
					// target is dead, find a new target or keep moving
					entity->status = EntityStatus::IDLE;
				}
				else
				{
					// Status is correct, attacking enemy within range
					return;
				}
			}
			else
			{
				// Lost target, reacquire or keep moving
				entity->status = EntityStatus::IDLE;
			}
		}
		EntityData* target = GetClosestEnemyWithinAggroRange(entity);
		if (target != nullptr)
		{
			float dx, dy, dz, dist;
			if (CheckRange(entity, target->position[0], target->position[1], target->position[2],
				entity->entityDefinition->size + target->entityDefinition->size, dx, dy, dz, dist))
			{
				entity->status = EntityStatus::ATTACKING;
				entity->attackTarget = target->entityId;
				entity->actionTime = 0;
			}
			else
			{
				entity->status = EntityStatus::MOVING;
				PathEntityToPoint(entity, target->position[0], target->position[1], target->position[2]);
			}
		}
		else
		{
			entity->status = EntityStatus::MOVING;
			PathEntityToPoint(entity, entity->commandData.targetX, entity->commandData.targetY, entity->commandData.targetZ);
		}
	}
	else if (entity->command == EntityCommand::STOP)
	{
		StopEntity(entity);
	}
	else if (entity->command == EntityCommand::NONE)
	{
		if (entity->status == EntityStatus::ATTACKING)
		{
			EntityData* target = entities[entity->attackTarget].get();
			float attackRange = entity->entityDefinition->size + target->entityDefinition->size + entity->entityDefinition->attackLeaveRange;

			float dx, dy, dz, dist;
			if (target->status == EntityStatus::DYING || target->status == EntityStatus::DEAD)
			{
				entity->status = EntityStatus::IDLE;
			}
			else if (CheckRange(entity, target->position[0], target->position[1], target->position[2],
				attackRange, dx, dy, dz, dist))
			{
				// Status is correct, attacking enemy within range
				return;
			}
			else
			{
				// Lost target, reacquire or keep moving
				entity->status = EntityStatus::IDLE;
			}
		}
		EntityData* target = GetClosestEnemyWithinAggroRange(entity);
		if (target != nullptr)
		{
			float dx, dy, dz, dist;
			if (CheckRange(entity, target->position[0], target->position[1], target->position[2],
				entity->entityDefinition->size + target->entityDefinition->size, dx, dy, dz, dist))
			{
				entity->status = EntityStatus::ATTACKING;
				entity->attackTarget = target->entityId;
				entity->actionTime = 0;
			}
			else
			{
				entity->status = EntityStatus::MOVING;
				PathEntityToPoint(entity, target->position[0], target->position[1], target->position[2]);
			}
		}
	}
	else if (entity->command == EntityCommand::HOLD)
	{
		if (entity->status == EntityStatus::ATTACKING)
		{
			EntityData* target = entities[entity->attackTarget].get();
			float dx, dy, dz, dist;
			float attackRange = entity->entityDefinition->size + target->entityDefinition->size + entity->entityDefinition->attackLeaveRange;

			if (target->status == EntityStatus::DYING || target->status == EntityStatus::DEAD)
			{
				entity->status = EntityStatus::IDLE;
			}
			else if (CheckRange(entity, target->position[0], target->position[1], target->position[2],
				attackRange, dx, dy, dz, dist))
			{
				// Status is correct, attacking enemy within range
				return;
			}
			else
			{
				// Lost target, reacquire or keep moving
				entity->status = EntityStatus::IDLE;
			}
		}
		EntityData* target = GetClosestEnemyWithinAggroRange(entity);
		if (target != nullptr)
		{
			float dx, dy, dz, dist;
			if (CheckRange(entity, target->position[0], target->position[1], target->position[2],
				entity->entityDefinition->size + target->entityDefinition->size, dx, dy, dz, dist))
			{
				entity->status = EntityStatus::ATTACKING;
				entity->attackTarget = target->entityId;
				entity->actionTime = 0;
			}
			else
			{
				entity->status = EntityStatus::IDLE;
			}
		}
	}
	else if (entity->command == EntityCommand::SPECIAL)
	{
		//todo
	}
}

void Game::UpdateGame(float elapsedTime)
{
	// 1. Set commands to update status
	for (auto it = entities.begin(); it != entities.end(); it++)
	{
		EntityData* entity = it->get();
		ApplyCommandToStatus(entity);
	}

	// 2. Update state based on current status
	for (auto it = entities.begin(); it != entities.end(); it++)
	{
		EntityData* entity = it->get();
	
		if (entity->status == EntityStatus::IDLE)
		{
			entity->velocity[0] = 0;
			entity->velocity[1] = 0;
			entity->velocity[2] = 0;
		}
		else if (entity->status == EntityStatus::MOVING)
		{
			if (!SetMoveToTargetPoint(entity, entity->targetPointX, entity->targetPointY,
				entity->targetPointZ, entity->entityDefinition->size))
			{
				entity->status = EntityStatus::IDLE;
			}
		}
		else if (entity->status == EntityStatus::ATTACKING)
		{
			entity->velocity[0] = 0;
			entity->velocity[1] = 0;
			entity->velocity[2] = 0;

			EntityData* target = entities[entity->attackTarget].get();
			float dx = target->position[0] - entity->position[0];
			float dy = target->position[1] - entity->position[1];
			float dz = target->position[2] - entity->position[2];
			float mag = sqrtf(dx * dx + dy * dy + dz * dz);
			SetOrientationToNormalizedVector(entity, dx / mag, dy / mag, dz / mag);

			bool alreadyHit = entity->actionTime >= entity->entityDefinition->attackHitTime;
			entity->actionTime += elapsedTime;
			bool hitNow = entity->actionTime >= entity->entityDefinition->attackHitTime;
			if (hitNow && !alreadyHit)
			{
				std::cout << entity->entityDefinition->name << " hit " << target->entityDefinition->name << std::endl;
				bool killed = DoAttack(entity, target);
				if (killed)
				{
					std::cout << entity->entityDefinition->name << " killed " << target->entityDefinition->name << std::endl;
					target->status = EntityStatus::DYING;
					target->command = EntityCommand::NONE;
					target->actionTime = 0;
					target->velocity[0] = 0;
					target->velocity[1] = 0;
					target->velocity[2] = 0;
				}
				else
				{
					// This is here to model hitstun.  But it causes loops of one unit repeatedly attacking.
				//	target->status = EntityStatus::ATTACKED;
				//	target->actionTime = 0;
				//	target->velocity[0] = 0;
				//	target->velocity[1] = 0;
				//	target->velocity[2] = 0;
				}
			}

			if (entity->actionTime >= entity->entityDefinition->attackDuration)
			{
				entity->actionTime -= entity->entityDefinition->attackDuration;
			}
		}
		else if (entity->status == EntityStatus::SPECIAL)
		{
			//todo
		}
		else if (entity->status == EntityStatus::ATTACKED)
		{
			entity->actionTime += elapsedTime;

			if (entity->actionTime >= entity->entityDefinition->attackedDuration)
			{
				entity->actionTime = 0;
				entity->status = EntityStatus::IDLE;
			}
		}
		else if (entity->status == EntityStatus::DYING)
		{
			entity->actionTime += elapsedTime;
			if (entity->actionTime >= entity->entityDefinition->deathDuration)
			{
				entity->actionTime = 0;
				entity->status = EntityStatus::DEAD;
			}
		}
		else if (entity->status == EntityStatus::DEAD)
		{
			// do nothing
		}
	}

	// 3. Move all entities after state has been set
	for (auto it = entities.begin(); it != entities.end(); it++)
	{
		EntityData* entity = it->get();
		if (entity->status == EntityStatus::DYING || entity->status == EntityStatus::DEAD)
		{
			continue;
		}
		float deltaX = entity->velocity[0] * elapsedTime;
		float deltaY = entity->velocity[1] * elapsedTime;
		float deltaZ = entity->velocity[2] * elapsedTime;
		float newPositionX = entity->position[0] + deltaX;
		float newPositionY = entity->position[1] + deltaY;
		float newPositionZ = entity->position[2] + deltaZ;
		
		// Model entities as circles around their position
		// check for collisions
		for (auto collider = entities.begin(); collider != entities.end(); collider++)
		{
			if (collider != it)
			{
				EntityData* target = collider->get();
				if (target->status != EntityStatus::DYING && target->status != EntityStatus::DEAD)
				{
					float dx = target->position[0] - newPositionX;
					float dy = target->position[1] - newPositionY;
					float dz = target->position[2] - newPositionZ;
					float mag = sqrt(dx * dx + dy * dy + dz * dz);
					if (mag < 1e-6)
					{
						//stuck - continue
						continue;
					}
					float dist = mag - (entity->entityDefinition->size + target->entityDefinition->size);
					if (dist < 0)
					{
						// went too far, pull back by dist
						float vnx = dx / mag;
						float vny = dy / mag;
						float vnz = dz / mag;
						newPositionX += vnx * dist;
						newPositionY += vny * dist;
						newPositionZ += vnz * dist;
					}
				}
			}
		}

		entity->position[0] = newPositionX;
		entity->position[1] = newPositionY;
		entity->position[2] = newPositionZ;
	}
}

void Game::ProcessCommand(int entityId, EntityCommand command, EntityCommandData commandData)
{
	EntityData* entity = entities[entityId].get();
	entity->command = command;
	entity->commandData = commandData;
}

void Game::WriteServerState(heroes::ServerState& state)
{
	state.clear_entities();
	for (auto it = entities.begin(); it != entities.end(); it++)
	{
		EntityData* e = it->get();
		heroes::Entity* entity = state.add_entities();

		entity->set_entitytype(e->entityDefinition->type);
		entity->set_controllingplayer(e->playerId);
		entity->set_id(e->entityId);
		entity->set_orientation(e->orientation);
		heroes::Entity_Status status;
		switch (e->status)
		{
		case EntityStatus::IDLE:
			status = heroes::Entity_Status_IDLE;
			break;

		case EntityStatus::MOVING:
			status = heroes::Entity_Status_MOVING;
			break;

		case EntityStatus::ATTACKING:
			status = heroes::Entity_Status_ATTACKING;
			break;

		case EntityStatus::SPECIAL:
			status = heroes::Entity_Status_SPECIAL;
			break;

		case EntityStatus::ATTACKED:
			status = heroes::Entity_Status_ATTACKED;
			break;

		case EntityStatus::DYING:
			status = heroes::Entity_Status_DYING;
			break;

		case EntityStatus::DEAD:
			status = heroes::Entity_Status_DEAD;
			break;
		
		default:
			status = heroes::Entity_Status_IDLE;
			break;
		}

		entity->set_status(status);
		entity->set_positionx(e->position[0]);
		entity->set_positiony(e->position[1]);
		entity->set_positionz(e->position[2]);

		entity->set_targetpointx(e->targetPointX);
		entity->set_targetpointy(e->targetPointY);
		entity->set_targetpointz(e->targetPointZ);
		entity->set_entitytype(e->entityDefinition->type);
	}
}