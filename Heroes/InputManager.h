#pragma once

class Game;
class Renderer;
class SelectionManager;

class InputManager
{
	Game* m_game;
	Renderer* m_renderer;
	SelectionManager* m_selectionManager;
	int leftClickX, leftClickY;
	int mouseX, mouseY;
	bool isDragging;
public:
	InputManager(Game* game, Renderer* renderer, SelectionManager* selectionManager);
	void HandleKey(int key);
	void HandleMouseMove(int x, int y);
	void HandleMouseWheel(int zDelta);
	void HandleMouseLeftButtonDown(int x, int y);
	void HandleMouseLeftButtonUp(int x, int y);
	void HandleMouseRightButtonDown(int x, int y);
	void HandleMouseRightButtonUp(int x, int y);

	// Called once per frame before render
	void Update(float elapsedTime);
	void Render();
};