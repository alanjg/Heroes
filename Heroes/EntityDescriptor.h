#pragma once
class EntityDescriptor
{
public:
	EntityDescriptor();
	~EntityDescriptor();

	bool hasName;
	std::string name;
	std::string alias;
	int startX, startY;
};

