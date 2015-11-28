#pragma once
class Player;

class PlayerController
{
protected:
	std::shared_ptr<Player> m_player;
public:
	PlayerController();
	virtual ~PlayerController();

	std::string faction;
	virtual void UpdateActions();
	void SetPlayer(std::shared_ptr<Player> player);
	Player* GetPlayer();
};