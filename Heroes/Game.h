#pragma once

class Entity;
class GameState;
class Renderer;
class SelectionManager;
class InputManager;
class ResourceManager;

class Game
{
private:
	std::shared_ptr<GameState> state;
	Renderer* m_renderer;
	InputManager* m_inputManager;
	SelectionManager* m_selectionManager;
	ResourceManager* m_resourceManager;	

public:
	Game();
	void SetContext(Renderer* renderer, SelectionManager* selectionManager, InputManager* inputManager, ResourceManager* resourceManager);
	std::vector<std::shared_ptr<Entity>> entities;
	void Update(float elapsedTime);
	void Render();

	void CreateTestGame();
};