#pragma once

class Player;

class Team
{
	std::vector<std::shared_ptr<Player>> players;
public:
	Team();
	~Team();

	std::vector<std::shared_ptr<Player>>& GetPlayers();
};

