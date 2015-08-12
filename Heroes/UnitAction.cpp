#include "stdafx.h"
#include "Unit.h"
#include "UnitAction.h"
#include "Entity.h"

UnitAction::UnitAction(Unit* unit) :m_unit(unit)
{
}

UnitAction::~UnitAction()
{
}

UnitMoveAction::UnitMoveAction(Unit* unit) : UnitAction(unit)
{
}

void UnitMoveAction::Update(float elapsedTime)
{
	float x = unit->position[0];
	float y = unit->position[1];
	float dx = targetX - x;
	float dy = targetY - y;
	float mag = sqrt(dx*dx + dy * dy);
	if (mag < 0.001)
	{
		return;
	}
	dx /= mag;
	dy /= mag;
	dx *= unit->speed * elapsedTime;
	dy *= unit->speed * elapsedTime;
	unit->position[0] += dx;
	unit->position[1] += dy;
}

bool UnitMoveAction::IsActive()
{
	return true;
}

UnitAttackAction::UnitAttackAction(Entity* target, Unit* unit) : UnitAction(unit), m_target(target)
{
}

void UnitAttackAction::Update(float elapsedTime)
{
	float x = m_unit->x;
	float y = m_unit->y;
	float dx = m_target->x - x;
	float dy = m_target->y - y;
	float mag = sqrt(dx*dx + dy * dy);
	if (mag <= 2)
	{
		if (m_unit->isAttacking)
		{
			return;
		}
		else
		{
			m_unit->Attack();
			return;
		}
	}
	else
	{
		dx /= mag;
		dy /= mag;

		float targetX = x + dx * (mag - 2);
		float targetY = y + dy * (mag - 2);
		m_unit->WalkToPoint(targetX, targetY);
	}
}

bool UnitAttackAction::IsActive()
{
	return true;
}