#include "stdafx.h"
#include "InputManager.h"
#include "Renderer.h"
#include "Game.h"
#include "Entity.h"
#include "Camera.h"
#include "SelectionManager.h"
#include "EntityAction.h"
#include "InputPlayerController.h"
#include "Player.h"
#include "WinMain.h"
#include "NetworkClient.h"

InputManager::InputManager(Game* game, Renderer* renderer, SelectionManager* selectionManager) :
	m_game(game), m_renderer(renderer), m_selectionManager(selectionManager)
{

}

void InputManager::BindPlayerController(InputPlayerController* playerController)
{
	m_playerController = playerController;
}

void InputManager::HandleKey(int key)
{
	switch (key)
	{
	case 's':
	case 'S':
		for each (auto e in m_selectionManager->selection)
		{
			if (g_useNetwork)
			{
				g_networkClient->SendStopCommand(e->entityId);
			}
			else
			{
				e->Stop();
			}
		}
		break;
	case 'h':
	case 'H':

		for each (auto e in m_selectionManager->selection)
		{
			if (g_useNetwork)
			{
				g_networkClient->SendHoldCommand(e->entityId);
			}
			else
			{
				e->Stop();
			}
		}
		break;
	case 'a':
	case 'A':
		for each (auto e in m_selectionManager->selection)
		{
			if (!g_useNetwork)
			{
				e->NextIdleAnimation();
			}
		}
	default:
		break;
	}
}

void InputManager::Update(float elapsedTime)
{
	float distance = 20 * elapsedTime;
	float dx = 0;
	float dz = 0;
	// move camera
	if (GetAsyncKeyState(VK_UP))
	{
		dz += distance;
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		dx -= distance;
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		dz -= distance;
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		dx += distance;
	}

	m_renderer->GetCamera()->Move(dx, 0, dz);
	m_renderer->GetCamera()->LookAt();
}

void InputManager::HandleMouseMove(int x, int y)
{
	mouseX = x;
	mouseY = y;
}

void InputManager::HandleMouseLeftButtonDown(int x, int y)
{
	leftClickX = x;
	leftClickY = y;
	mouseX = x;
	mouseY = y;
	isDragging = true;
}

void InputManager::HandleMouseLeftButtonUp(int x, int y)
{
	if (x == leftClickX && y == leftClickY)
	{
		Entity* hitElement = m_renderer->Pick(x, y);
		if (hitElement != nullptr)
		{
			m_selectionManager->SetSelection(hitElement);
		}
	}
	else
	{
		int sx = std::min(x, leftClickX);
		int sw = abs(x - leftClickX);
		int sy = std::min(y, leftClickY);
		int sh = abs(y - leftClickY);
		std::vector<Entity*> hitElements;
		m_renderer->Pick(sx, sy, sw, sh, hitElements);
		m_selectionManager->SetSelection(hitElements);
	}
	isDragging = false;
}

void InputManager::HandleMouseRightButtonDown(int x, int y)
{
	Entity* hitElement = m_renderer->Pick(x, y);
	if (hitElement != nullptr) // disable hitting other elements for now
	{
		// attack hit element
		for each(auto e in m_selectionManager->selection)
		{
			for each (auto p in m_game->entities)
			//for each(auto p in m_playerController->GetPlayer()->GetEntities())
			{
				if (p.get() == e)
				{
					if (e->CanMove())
					{
						if (g_useNetwork)
						{
							// send command to move to x
							g_networkClient->SendAttackUnitCommand(e->entityId, hitElement->entityId);
						}
						else
						{
							e->Stop();
							e->RangeAttack(hitElement->x, hitElement->y);
							//std::shared_ptr<EntityAction> action(new EntityAttackAction(hitElement, e));
							//e->currentAction = action;
						}

					}
					break;
				}
			}
		}
	}
	else
	{
		XMFLOAT4 planePointf;
		planePointf.x = 0;
		//planePointf.y = 0; 
		planePointf.y = -1.8f;// Offset to align with model offset
		planePointf.z = 0;
		planePointf.w = 1.0;
		XMVECTOR planePoint = XMLoadFloat4(&planePointf);
		XMFLOAT4 planeNormalf;
		planeNormalf.x = 0;
		planeNormalf.y = 1;
		planeNormalf.z = 0;
		planeNormalf.w = 0;
		XMVECTOR planeNormal = XMLoadFloat4(&planeNormalf);

		XMFLOAT4 pf;
		XMVECTOR p;
		if (m_renderer->ProjectPickingRayToPlane(x, y, planePoint, planeNormal, p))
		{
			XMStoreFloat4(&pf, p);
			for each(auto e in m_selectionManager->selection)
			{
//				for each(auto p in m_playerController->GetPlayer()->GetEntities())
				for each (auto p in m_game->entities)
				{
					if (p.get() == e)
					{
						if (e->CanMove())
						{
							if (g_useNetwork)
							{
								// send command to move to x
								g_networkClient->SendMoveCommand(e->entityId, pf.x, pf.z);
							}
							else
							{
								e->Stop();
								e->WalkToPoint(pf.x, pf.z);
							}
						}
					}
				}
			}
		}
	}
}

void InputManager::HandleMouseRightButtonUp(int x, int y)
{
}

void InputManager::HandleMouseWheel(int zDelta)
{
	m_renderer->GetCamera()->Zoom(-zDelta * .02f);
	m_renderer->GetCamera()->LookAt();
}

void InputManager::Render()
{
	if (isDragging)
	{
		m_renderer->DrawRect(std::min(leftClickX, mouseX), std::min(leftClickY, mouseY), std::max(leftClickX, mouseX), std::max(leftClickY, mouseY));
	}
}