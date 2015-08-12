#pragma once
#include "PlayerController.h"

class AIPlayerController :
	public PlayerController
{
public:
	AIPlayerController();
	virtual ~AIPlayerController();

	virtual void UpdateActions();
};

