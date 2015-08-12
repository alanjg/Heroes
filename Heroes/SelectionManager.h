#pragma once

class Renderer;
class Entity;
class StaticMeshInstance;

class SelectionManager
{
	Renderer* m_renderer;
	std::vector<StaticMeshInstance*> selectionMeshes;
	void UpdateSelectionMeshes();
public:
	SelectionManager(Renderer* renderer);
	~SelectionManager();

	std::vector<Entity*> selection;
	

	void SetSelection(Entity* e);
	void SetSelection(const std::vector<Entity*>& e);
	void ClearSelection();
	void ToggleSelection(Entity* e);

	void Update(float elapsedTime);
	void Render();
};

