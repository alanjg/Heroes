#pragma once
#include "ActionDefinition.h"

struct EntityDefinition
{
	int type;
	std::string name;
	std::string alias;
	std::string modelName;

	// structure attributes
	float width;
	float height;

	bool canMove;
	float moveSpeed;

	// unit attributes for combat
	float size;

	// Melee attack
	float attackHitTime;
	float attackDuration;
	std::string meleeAttackName;

	// can still hit within this range once attack starts
	float attackLeaveRange;
	int attackValue;
	int damageLow, damageHigh;

	// Range attack
	bool hasRangedAttack;
	float rangedAttackRange;
	float rangedAttackHitTime;
	float rangedAttackDuration;
	std::string rangedAttackName;

	float attackedDuration;
	float deathDuration;
	
	float aggroRange;
	
	int defenseValue;
	int hitPoints;

	
	std::vector<EntityDefinition*> productionOptions;
	//std::vector<std::unique_ptr<ActionDefinition>> specialActions;
	//ActionDefinition* rangedAttack;
	//ActionDefinition* meleeAttack;
};