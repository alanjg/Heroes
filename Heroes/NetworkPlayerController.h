#pragma once
#include "PlayerController.h"
class NetworkPlayerController :
	public PlayerController
{
public:
	NetworkPlayerController();
	virtual ~NetworkPlayerController();
};

