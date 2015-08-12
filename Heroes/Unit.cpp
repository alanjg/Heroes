#include "stdafx.h"
#include "Unit.h"
#include "SkinnedMeshInstance.h"
#include "UnitAction.h"

using namespace DirectX;

Unit::Unit()
{
	walkSpeed = 4.0;
	turnSpeed = 3.0;
}

Unit::~Unit()
{
}

void Unit::Initialize(float xx, float yy, SkinnedMeshInstance* m)
{
	x = xx;
	y = yy;
	mesh = m;
	m_mesh = m;
	orientation = 0;
	walkProgress = 0;
	idleProgress = 0.0;
	isTurning = false;
	isWalking = false;
	isAttacking = false;
}

void Unit::WalkToPoint(float tx, float ty)
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

void Unit::Attack()
{
	isTurning = false;
	isWalking = false;
	isAttacking = true;
	attackProgress = 0;
}

void Unit::Stop()
{
	isTurning = false;
	isWalking = false;
	isAttacking = false;
	currentAction = nullptr;
}

void Unit::Update(float elapsedTime)
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
	mesh->SetTransform(w);
	
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
		mesh->SetAnimation("move", t);
	}
	else if (isAttacking)
	{
		mesh->SetAnimation("attack00", t);
	}
	else
	{
		mesh->SetAnimation("idle00", t);
	}
}