#pragma once

class Entity;
class GameState;

class Game
{
private:
	std::shared_ptr<GameState> state;
public:

	std::vector<std::shared_ptr<Entity>> entities;
	void Update(float elapsedTime);
	void Render();

	void CreateTestGame();
};