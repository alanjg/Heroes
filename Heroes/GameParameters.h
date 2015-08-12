#pragma once
class PlayerController;

class GameParameters
{
public:
	GameParameters();
	~GameParameters();

	std::vector<std::shared_ptr<PlayerController>> playerControllers;
};

