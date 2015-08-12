#pragma once
class PlayerController
{
public:
	PlayerController();
	virtual ~PlayerController();

	std::string faction;
	virtual void UpdateActions();
};

