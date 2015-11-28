#pragma once
class MeshInstance;
class SkinnedMeshInstance;
class EntityAction;

#include "EntityDefinition.h"
class Entity
{
	EntityDefinition& entityDefintition;
public:
	Entity(EntityDefinition& definition);
	~Entity();

	bool isAnimatedMesh;
	MeshInstance* m_mesh;
	float x, y;
	float orientation;
	
	std::string name;
	std::string alias;
	float position[3];
	float velocity[3];

	std::shared_ptr<EntityAction> currentAction;

	// movement and animation variables
	float animationAnchorX, animationAnchorY;
	float targetX, targetY;
	float targetOrientation, turnDirection;
	SkinnedMeshInstance* m_skinnedMesh;
	float walkProgress;
	float idleProgress;
	float attackProgress;

	float walkSpeed, turnSpeed;
	bool isTurning;
	bool isWalking;
	bool isAttacking;

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
	void Stop();

	virtual void Initialize(float startX, float startY, float startOrientation, MeshInstance* m);
	void Render();
	virtual void Update(float time);
};

