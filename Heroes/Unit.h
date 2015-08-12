#pragma once
#include "Entity.h"

class UnitAction;
class SkinnedMeshInstance;

class Unit : public Entity
{
public:
	Unit();
	~Unit();

	float size;
	float speed;
	int attack;
	int defense;
	int hitPoints;

	std::shared_ptr<UnitAction> currentAction;

	float animationAnchorX, animationAnchorY;
	float targetX, targetY;
	float targetOrientation, turnDirection;
	SkinnedMeshInstance* mesh;
	float walkProgress;
	float idleProgress;
	float attackProgress;

	float walkSpeed, turnSpeed;
	bool isTurning;
	bool isWalking;
	bool isAttacking;

	virtual void Initialize(float xx, float yy, SkinnedMeshInstance* m);
	
	// Actions
	void WalkToPoint(float tx, float ty);
	void Attack();
	void Stop();

	void Render();
	void Update(float time);
};

