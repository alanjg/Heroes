#pragma once

#include "EntityDescriptor.h"

class PlayerStartLocation
{
public:
	PlayerStartLocation();
	~PlayerStartLocation();

	int startX, startY;
	std::vector<std::shared_ptr<EntityDescriptor>> entities;
};

