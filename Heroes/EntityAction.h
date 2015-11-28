#pragma once

class Entity;

enum EntityActionType
{
	EntityNoOrders,
	EntityStop,
	EntityMove,
	EntityAttackMove,
	EntityAttack,
	EntityBuild,
	EntityHoldPosition,
	EntityRepair
};

EntityActionType ParseEntityActionType(const std::string& action);

class EntityAction
{
protected:
	Entity* m_entity;
public:
	EntityAction(Entity* entity);
	virtual ~EntityAction();

	virtual void Update(float elapsedTime) = 0;
	virtual bool IsActive() = 0;
};

struct EntityMoveWaypoint
{
	int gridX, gridY;
};

class EntityMoveAction : public EntityAction
{
	std::vector<EntityMoveWaypoint> path;
public:
	EntityMoveAction(Entity* entity);
	float targetX, targetY;

	virtual void Update(float elapsedTime);
	virtual bool IsActive();
};

class EntityAttackAction : public EntityAction
{
	Entity* m_target;
public:
	EntityAttackAction(Entity* target, Entity* source);
	virtual void Update(float elapsedTime);
	virtual bool IsActive();
};

class EntityAttackMoveAction : public EntityAction
{

};