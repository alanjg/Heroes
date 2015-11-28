#include "stdafx.h"
#include "Entity.h"
#include "EntityAction.h"

EntityActionType ParseEntityActionType(const std::string& action)
{
	if (action == "Stop") return EntityStop;
	if (action == "Move") return EntityMove;
	if (action == "Attack") return EntityAttack;
	if (action == "Build") return EntityBuild;
	if (action == "Hold") return EntityHoldPosition;
	if (action == "Repair") return EntityRepair;
	return EntityNoOrders;
}

EntityAction::EntityAction(Entity* entity) :m_entity(entity)
{
}

EntityAction::~EntityAction()
{
}

EntityMoveAction::EntityMoveAction(Entity* entity) : EntityAction(entity)
{
}

void EntityMoveAction::Update(float elapsedTime)
{
	float x = m_entity->position[0];
	float y = m_entity->position[1];
	float dx = targetX - x;
	float dy = targetY - y;
	float mag = sqrt(dx*dx + dy * dy);
	if (mag < 0.001)
	{
		return;
	}
	dx /= mag;
	dy /= mag;
	dx *= m_entity->speed * elapsedTime;
	dy *= m_entity->speed * elapsedTime;
	m_entity->position[0] += dx;
	m_entity->position[1] += dy;
}

bool EntityMoveAction::IsActive()
{
	return true;
}

EntityAttackAction::EntityAttackAction(Entity* target, Entity* entity) : EntityAction(entity), m_target(target)
{
}

void EntityAttackAction::Update(float elapsedTime)
{
	float x = m_entity->x;
	float y = m_entity->y;
	float dx = m_target->x - x;
	float dy = m_target->y - y;
	float mag = sqrt(dx*dx + dy * dy);
	if (mag <= 2)
	{
		if (m_entity->isAttacking)
		{
			return;
		}
		else
		{
			m_entity->Attack();
			return;
		}
	}
	else
	{
		dx /= mag;
		dy /= mag;

		float targetX = x + dx * (mag - 2);
		float targetY = y + dy * (mag - 2);
		m_entity->WalkToPoint(targetX, targetY);
	}
}

bool EntityAttackAction::IsActive()
{
	return true;
}