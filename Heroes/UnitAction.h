#pragma once

class Unit;

enum UnitActionType
{
	UnitNoOrders,
	UnitMove,
	UnitAttackMove,
	UnitAttack,
	UnitBuild,
	UnitHoldPosition,
	UnitRepair
};

class UnitAction
{
protected:
	Unit* m_unit;
public:
	UnitAction(Unit* m_unit);
	virtual ~UnitAction();

	virtual void Update(float elapsedTime) = 0;
	virtual bool IsActive() = 0;
};

struct UnitMoveWaypoint
{
	int gridX, gridY;
};

class UnitMoveAction : public UnitAction
{
	std::vector<UnitMoveWaypoint> path;
public:
	UnitMoveAction(Unit* unit);
	float targetX, targetY;
	std::shared_ptr<Unit> unit;

	virtual void Update(float elapsedTime);
	virtual bool IsActive();
};

class UnitAttackAction : public UnitAction
{
	Entity* m_target;
public:
	UnitAttackAction(Entity* target, Unit* unit);
	virtual void Update(float elapsedTime);
	virtual bool IsActive();
};