#pragma once

struct MapCell
{
	MapCell();
	int blockCount; // denotes how many units are blocking this cell.
	int visited; // used to track nodes examined in the search 
};

struct PathNode
{
	PathNode(float x, float y);
	float x, y;
};

struct SearchCell
{
	int x, y;
	SearchCell* prev;
};

struct SearchNode
{
	SearchNode();
	SearchNode(int x, int y, float cost, float heuristic);
	int x, y;
	float cost;
	float heuristic;
	SearchCell* cell;
	bool operator<(const SearchNode& rhs) const;
};

struct Path
{
	// A path is a set of waypoints.
	std::vector<PathNode> nodes;
};

class MapGrid
{
	int generation;
	std::vector<std::vector<MapCell>> map;
	int xMin, xMax, yMin, yMax, width, height;
	void ToGridCoord(float x, float y, float& gx, float& gy);
	void ToWorldCoord(float gx, float gy, float& x, float& y);
	bool IsFree(float x, float y, float size);
	void ClearVisited();
public:
	MapGrid(int minx, int miny, int maxx, int maxy);
	void AddUnit(float x, float y, float size);
	void RemoveUnit(float x, float y, float size);
	Path GetPathToPoint(float startX, float startY, float size, float endX, float endY);
};

