#include "stdafx.h"
#include "Entity.h"
#include "MeshInstance.h"
#include "SkinnedMeshInstance.h"
#include "EntityAction.h"

using namespace DirectX;

Entity::Entity(EntityDefinition& definition): entityDefintition(definition)
{	
	walkSpeed = 4.0;
	turnSpeed = 3.0;
}

Entity::~Entity()
{
}

void Entity::Render()
{
	m_mesh->RenderModel();
}

void Entity::Initialize(float startX, float startY, float startOrientation, MeshInstance* m)
{
	x = startX;
	y = startY;
	m_mesh = m;
	m_skinnedMesh = dynamic_cast<SkinnedMeshInstance*>(m);
	orientation = startOrientation / 360 * (2*XM_PI);
	walkProgress = 0;
	idleProgress = 0.0;
	isTurning = false;
	isWalking = false;
	isAttacking = false;
}

void Entity::Update(float elapsedTime)
{
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
		if (isAttacking)
		{
			attackProgress += remainingTime;
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
		else
		{
			m_skinnedMesh->SetAnimation("idle00", t);
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

void Entity::Attack()
{
	isTurning = false;
	isWalking = false;
	isAttacking = true;
	attackProgress = 0;
}

void Entity::Stop()
{
	isTurning = false;
	isWalking = false;
	isAttacking = false;
	currentAction = nullptr;
}

bool Entity::CanMove()
{
	return std::find(entityDefintition.actions.begin(), entityDefintition.actions.end(), EntityMove) != entityDefintition.actions.end();
}