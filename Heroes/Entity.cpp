#include "stdafx.h"
#include "Entity.h"
#include "MeshInstance.h"
#include "SkinnedMeshInstance.h"
#include "StaticMeshInstance.h"
#include "EntityAction.h"
#include "WinMain.h"
#include "Renderer.h"
#include "Game.h"
using namespace DirectX;

Entity::Entity()
{	
	walkSpeed = 4.0;
	turnSpeed = 3.0;
	idleAnimation = -1;
}

Entity::~Entity()
{
}

void Entity::Render()
{
	m_mesh->RenderModel();
	if (rangeAttackMesh != nullptr)
	{
		rangeAttackMesh->RenderModel();
	}
}

void Entity::Initialize(float startX, float startY, float startOrientation, MeshInstance* m)
{
	x = startX;
	y = startY;
	m_mesh = m;
	m_skinnedMesh = dynamic_cast<SkinnedMeshInstance*>(m);
	rangeAttackMesh = nullptr;
	
	orientation = startOrientation / 360 * (2*XM_PI);
	walkProgress = 0;
	idleProgress = 0.0;
	attackedProgress = 0;
	attackProgress = 0;
	dieProgress = 0;
	isTurning = false;
	isAttacked = false;
	isWalking = false;
	isAttacking = false;
	isDying = false;
	isDead = false;
	isRangeAttacking = false;
}

void Entity::Update(float elapsedTime)
{
	if (rangeAttackMesh != nullptr)
	{
		rangeAttackProgress += elapsedTime;
		if (rangeAttackProgress < rangeAttackTotalTime)
		{
			rangeAttackX += rdx * elapsedTime;
			rangeAttackY += rdy * elapsedTime;

			XMMATRIX world = XMMatrixRotationX(-XM_PIDIV2) * XMMatrixRotationY(rangeAttackOrientation + XM_PIDIV2) * XMMatrixTranslation(-0.137041f, -1.8f, -1.06089f) * XMMatrixTranslation(rangeAttackX, 0, rangeAttackY);
			XMFLOAT4X4 w;
			XMStoreFloat4x4(&w, world);
			rangeAttackMesh->SetTransform(w);
		}
	}

	if (m_skinnedMesh == nullptr)
	{
		XMMATRIX world = XMMatrixRotationX(-XM_PIDIV2) * XMMatrixRotationY(orientation + XM_PIDIV2) * XMMatrixTranslation(-0.137041f, -1.8f, -1.06089f) * XMMatrixTranslation(x, 0, y);
		XMFLOAT4X4 w;
		XMStoreFloat4x4(&w, world);
		m_mesh->SetTransform(w);
	}
	else
	{
		if (currentAction != nullptr)
		{
			currentAction->Update(elapsedTime);
			if (!currentAction->IsActive())
			{
				currentAction = nullptr;
			}
		}

		float remainingTime = elapsedTime;
		if (isAttacking)
		{
			attackProgress += remainingTime;
			remainingTime = 0;
		}
		else if (isAttacked)
		{
			attackedProgress += remainingTime;
			remainingTime = 0;
		}
		else if (isDying)
		{
			dieProgress += remainingTime;
			remainingTime = 0;
		}

		if (isTurning)
		{
			float remainingTurn = targetOrientation - orientation;
			float turnAmount = remainingTime * turnSpeed;
			if (abs(remainingTurn) < turnAmount)
			{
				float ratio = abs(remainingTurn) / turnAmount;
				remainingTime *= (1 - ratio);
				orientation = targetOrientation;
				isWalking = true;
				isTurning = false;
				walkProgress = 0;
			}
			else
			{
				orientation += turnAmount * turnDirection;
				walkProgress += remainingTime;
				remainingTime = 0;
			}
		}
		if (isWalking)
		{
			float dx = targetX - x;
			float dy = targetY - y;
			float walkAmount = walkSpeed * remainingTime;
			float d2 = dx*dx + dy * dy;
			float d = sqrt(d2);
			if (walkAmount >= d)
			{
				x = targetX;
				y = targetY;
				float ratio = d / walkAmount;
				remainingTime *= (1 - ratio);
				isWalking = false;
				walkProgress = 0;
			}
			else
			{
				/*
				float timeToFullWalkCycle = walkCycleDuration - walkProgress;
				if (remainingTime >= timeToFullWalkCycle)
				{
				float cycleWalkAmount = walkSpeed * timeToFullWalkCycle;
				animationAnchorX = x + dx / d * cycleWalkAmount;
				animationAnchorY = y + dy / d * cycleWalkAmount;
				}
				*/

				x += dx / d * walkAmount;
				y += dy / d * walkAmount;

				walkProgress += remainingTime;
				remainingTime = 0;
			}
		}
		
		if (!isTurning && !isWalking && !isAttacking)
		{
			idleProgress += remainingTime;
		}

		float currentX, currentY;
		if (isWalking && false)
		{
			currentX = animationAnchorX;
			currentY = animationAnchorY;
		}
		else
		{
			currentX = x;
			currentY = y;
		}
		XMMATRIX world = XMMatrixRotationX(-XM_PIDIV2) * XMMatrixRotationY(orientation + XM_PIDIV2) * XMMatrixTranslation(-0.137041f, -1.8f, -1.06089f) * XMMatrixTranslation(currentX, 0, currentY);
		
		XMFLOAT4X4 w;
		XMStoreFloat4x4(&w, world);
		m_skinnedMesh->SetTransform(w);

		float t = 0;
		if (isWalking)
		{
			t = walkProgress;
		}
		else if (isTurning)
		{
			t = walkProgress;
		}
		else if (isAttacking)
		{
			t = attackProgress;
		}
		else if (isAttacked)
		{
			t = attackedProgress;
		}
		else if (isDying)
		{
			t = dieProgress;
		}
		else
		{
			t = idleProgress;
		}
		if (isWalking)
		{
			m_skinnedMesh->SetAnimation("move", t);
		}
		else if (isAttacking)
		{
			m_skinnedMesh->SetAnimation("attack00", t);
		}
		else if (isRangeAttacking)
		{
			m_skinnedMesh->SetAnimation("rangeattack", idleProgress);
		}
		else if (isAttacked)
		{
			m_skinnedMesh->SetAnimation("hit", t);
		}
		else if (isDying)
		{
			m_skinnedMesh->SetAnimation("death", t, false);
		}
		else
		{
			if (idleAnimation == -1)
			{
				m_skinnedMesh->SetAnimation("idle00", t);
			}
			else
			{
				m_skinnedMesh->SetAnimation(idleAnimation, t);
			}
		}
	}
}

void Entity::WalkToPoint(float tx, float ty)
{
	isAttacking = false;

	targetX = tx;
	targetY = ty;
	float dx = tx - x;
	float dy = ty - y;

	if (abs(dx) < 1e-6 && abs(dy) < 1e-6)
	{
		isTurning = false;
		isWalking = false;
		return;
	}
	animationAnchorX = x;
	animationAnchorY = y;
	targetOrientation = atan2(-dy, dx);
	float deltaTurn = targetOrientation - orientation;
	float deltaTurnDist = abs(deltaTurn);
	bool flipped = false;
	if (deltaTurnDist > XM_PI)
	{
		if (targetOrientation < orientation)
		{
			targetOrientation += 2 * XM_PI;
		}
		else
		{
			orientation += 2 * XM_PI;
		}
		deltaTurn = targetOrientation - orientation;
	}

	turnDirection = deltaTurn < 0 ? -1.0f : 1.0f;
	deltaTurnDist = abs(deltaTurn);
	if (deltaTurnDist < 1e-6)
	{
		isTurning = false;
		isWalking = true;
	}
	else
	{
		isTurning = true;
		isWalking = false;
	}
}


void Entity::RangeAttack(float tx, float ty)
{
	if (rangeAttackMesh)
	{
		delete rangeAttackMesh;
	}
	isTurning = false;
	isWalking = false;
	isAttacking = false;
	isAttacked = false;
	isDying = false;
	isDead = false;
	isRangeAttacking = true;
	rangeAttackX = x;
	rangeAttackY = y;
	rdx = tx - x;
	rdy = ty - y;
	
	rangeAttackOrientation = orientation;
	rangeAttackTotalTime = 1;
	rangeAttackProgress = 0;
	//float dist = sqrt(rdx * rdx + rdy * rdy);
	rdx /= rangeAttackTotalTime;
	rdy /= rangeAttackTotalTime;
	rangeAttackMesh = g_renderer->CreateStaticMeshInstance("Archershot");
	attackProgress = 0;
}

void Entity::Attack()
{
	//m_skinnedMesh->PlayAnimationSound("attack00");
	isTurning = false;
	isWalking = false;
	isAttacking = true;
	isAttacked = false;
	isDying = false;
	isDead = false;
	attackProgress = 0;
}

void Entity::Stop()
{
	isTurning = false;
	isWalking = false;
	isAttacking = false;
	isAttacked = false;
	isDying = false;
	isDead = false;
	currentAction = nullptr;
}

void Entity::Die()
{
	isTurning = false;
	isWalking = false;
	isAttacking = false;
	isAttacked = false;
	isDying = true;
	isDead = false;
}

void Entity::GetHit()
{
	isTurning = false;
	isWalking = false;
	isAttacking = false;
	isAttacked = true;
	isDying = false;
	isDead = false;
}

bool Entity::CanMove()
{
	return canMove;
}

void Entity::NextIdleAnimation()
{
	idleAnimation++;
}