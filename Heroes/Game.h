#pragma once

class Entity;
class Game
{
private:
public:

	std::vector<std::shared_ptr<Entity>> entities;
	void Update(float elapsedTime);
	void Render();
};