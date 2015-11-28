#pragma once
class EntityDescriptor
{
public:
	EntityDescriptor();
	~EntityDescriptor();

	std::string name;
	std::string alias;
	int startX, startY;
	int orientation;
};

