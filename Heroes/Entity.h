#pragma once
class MeshInstance;
#include "EntityDefinition.h"
class Entity
{
	EntityDefinition entityDefintition;
public:
	Entity();
	~Entity();

	MeshInstance* m_mesh;
	float x, y;
	//float animationAnchorX, animationAnchorY;
	//float targetX, targetY;
	float orientation;
	//MeshInstance* mesh;
	//float walkProgress;
	//float idleProgress;

	//float walkSpeed, turnSpeed;
	//bool isTurning;
	//bool isWalking;

	std::string name;
	std::string alias;
	float position[3];
	float velocity[3];
	int hitPoints;
	int defense;

	virtual void Initialize(float xx, float yy, MeshInstance* m);
	void Render();
	virtual void Update(float time);
};

