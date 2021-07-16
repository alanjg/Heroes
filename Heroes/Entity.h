#pragma once
class MeshInstance;
class StaticMeshInstance;
class SkinnedMeshInstance;
class EntityAction;

//#include "EntityDefinition.h"
#include "../GameCore/EntityData.h"

class Entity
{
public:
	Entity();
	~Entity();

	bool isAnimatedMesh;
	MeshInstance* m_mesh;
	float x, y;
	float orientation;
	
	std::string name;
	std::string alias;
	float position[3];
	float velocity[3];
	int entityId;
	EntityData entityData;
	bool canMove;

	std::shared_ptr<EntityAction> currentAction;
	int idleAnimation;

	// movement and animation variables
	float animationAnchorX, animationAnchorY;
	float targetX, targetY;
	float targetOrientation, turnDirection;
	SkinnedMeshInstance* m_skinnedMesh;
	float walkProgress;
	float idleProgress;
	float attackProgress;
	float attackedProgress;
	float dieProgress;

	StaticMeshInstance* rangeAttackMesh;
	float rangeAttackX, rangeAttackY, rangeAttackOrientation;
	float rdx, rdy;
	float rangeAttackTotalTime, rangeAttackProgress;


	float walkSpeed, turnSpeed;
	bool isTurning;
	bool isWalking;
	bool isAttacking;
	bool isRangeAttacking;
	bool isAttacked;
	bool isDying;
	bool isDead;

	// unit attributes for combat
	float size;
	float speed;
	int attack;
	int defense;
	int hitPoints;

	// Actions
	bool CanMove();
	void WalkToPoint(float tx, float ty);
	void Attack();
	void RangeAttack(float tx, float ty);
	void GetHit();
	void Stop();
	void Die();
	void NextIdleAnimation();

	virtual void Initialize(float startX, float startY, float startOrientation, MeshInstance* m);
	void Render();
	virtual void Update(float time);

	// Update animation for current values of x, y, orientation, position
};