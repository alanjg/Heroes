#include "stdafx.h"
#include "InputManager.h"
#include "Renderer.h"
#include "Game.h"
#include "Entity.h"
#include "Unit.h"
#include "Camera.h"
#include "SelectionManager.h"
#include "UnitAction.h"

InputManager::InputManager(Game* game, Renderer* renderer, SelectionManager* selectionManager) :
	m_game(game), m_renderer(renderer), m_selectionManager(selectionManager)
{

}

void InputManager::HandleKey(int key)
{
	switch (key)
	{
	case ' ':
		m_renderer->play = !m_renderer->play;
		break;
	case 'P':
		m_renderer->writeModel = true;
		break;
	case 190: //'.':
		m_renderer->animationIndex++;
		break;
	case 188: //',':
		m_renderer->animationIndex--;
		break;
	/*case '1':
		m_game->entities[0]->WalkToPoint(-3, 0);
		break;
	case '2':
		m_game->entities[0]->WalkToPoint(3, 0);
		break;
	case '3':
		m_game->entities[0]->WalkToPoint(3, 3);
		break;
	case '4':
		m_game->entities[0]->WalkToPoint(-3, 3);
		break;
	case '5':
		m_game->entities[0]->WalkToPoint(0, 3);
		break;
	case '6':
		m_game->entities[1]->WalkToPoint(-3, 0);
		break;
	case '7':
		m_game->entities[1]->WalkToPoint(3, 0);
		break;
	case '8':
		m_game->entities[1]->WalkToPoint(3, 3);
		break;
	case '9':
		m_game->entities[1]->WalkToPoint(-3, 3);
		break;
	case '0':
		m_game->entities[1]->WalkToPoint(0, 3);
		break;*/
	}
}


void InputManager::Update(float elapsedTime)
{
	float distance = 20 * elapsedTime;
	float dx = 0;
	float dz = 0;
	// move camera
	if (GetAsyncKeyState('W'))
	{
		dz += distance;
	}
	if (GetAsyncKeyState('A'))
	{
		dx -= distance;
	}
	if (GetAsyncKeyState('S'))
	{
		dz -= distance;
	}
	if (GetAsyncKeyState('D'))
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
	if (hitElement != nullptr)
	{
		// attack hit element
		for each(auto e in m_selectionManager->selection)
		{
			Unit* unit = dynamic_cast<Unit*>(e);
			if (unit != nullptr)
			{
				std::shared_ptr<UnitAction> action(new UnitAttackAction(hitElement, unit));
				unit->currentAction = action;
			}
		}
	}
	else
	{
		XMFLOAT4 planePointf;
		planePointf.x = 0;
		planePointf.y = 0;
		planePointf.z = 0;
		XMVECTOR planePoint = XMLoadFloat4(&planePointf);
		XMFLOAT4 planeNormalf;
		planeNormalf.x = 0;
		planeNormalf.y = 1;
		planeNormalf.z = 0;
		XMVECTOR planeNormal = XMLoadFloat4(&planeNormalf);

		XMFLOAT4 pf;
		XMVECTOR p;
		if (m_renderer->ProjectPickingRayToPlane(x, y, planePoint, planeNormal, p))
		{
			XMStoreFloat4(&pf, p);
			for each(auto e in m_selectionManager->selection)
			{
				Unit* unit = dynamic_cast<Unit*>(e);
				if (unit != nullptr)
				{
					unit->Stop();
					unit->WalkToPoint(pf.x, pf.z);
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