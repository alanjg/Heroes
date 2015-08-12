#include "stdafx.h"
#include "SelectionManager.h"
#include "Renderer.h"
#include "StaticMeshInstance.h"
#include "Entity.h"

SelectionManager::SelectionManager(Renderer* renderer) : m_renderer(renderer)
{
}

SelectionManager::~SelectionManager()
{
}

void SelectionManager::SetSelection(Entity* e)
{
	selection.clear();
	selection.push_back(e);
	UpdateSelectionMeshes();
}

void SelectionManager::SetSelection(const std::vector<Entity*>& e)
{
	selection.clear();
	selection.assign(e.begin(), e.end());
	UpdateSelectionMeshes();
}

void SelectionManager::ClearSelection()
{
	selection.clear();
	UpdateSelectionMeshes();
}

void SelectionManager::ToggleSelection(Entity* e)
{
	auto it = std::find(selection.begin(), selection.end(), e);
	if (it != selection.end())
	{
		selection.erase(it);
	}
	else
	{
		selection.push_back(e);
	}
	UpdateSelectionMeshes();
}

void SelectionManager::UpdateSelectionMeshes()
{
	selectionMeshes.clear();
	for each(auto m in selection)
	{
		StaticMeshInstance* mesh = m_renderer->CreateStaticMeshInstance("ArmySelection");
		if (mesh != nullptr)
		{
			selectionMeshes.push_back(mesh);
		}
	}	
}

void SelectionManager::Update(float elapsedTime)
{
	for (unsigned int i = 0; i < selection.size(); i++)
	{
		Entity* entity = selection[i];
		selectionMeshes[i]->SetTransform(entity->m_mesh->GetTransform());
	}
}

void SelectionManager::Render()
{
	for each(auto m in selectionMeshes)
	{
		m->RenderModel();
	}
}